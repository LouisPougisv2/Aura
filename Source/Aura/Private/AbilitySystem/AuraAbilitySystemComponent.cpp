// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Datas/AbilityInfo.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"

void UAuraAbilitySystemComponent::OnAbilityInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientOnGameplayEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartUpAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if(const UAuraGameplayAbility* AuraAbility = CastChecked<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartUpInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bAreStartupAbilitiesGiven = true;
	OnAbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> PassiveAbilityClass : StartUpPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(PassiveAbilityClass, 1);
		AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;

	//The perfect way to get abilities that are activatable!
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			//Setting the boolean on the AbilitySpec that is keeping track of whether or not its particular input is pressed
			AbilitySpecInputPressed(AbilitySpec);
			if(!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;

	//The perfect way to get abilities that are activatable!
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	//As Ability can become unactivatable during the execution, we lock them for the duration of the following loop
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if(!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if(GetAvatarActor()->Implements<UPlayerInterface>() && IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
	{
		//Upgrading the Attribute from the server
		ServerUpgradeAttribute(AttributeTag);
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTagToUpgrade)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTagToUpgrade;
	Payload.EventMagnitude = 1.0f;

	//Sending an event to an actor on the server
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTagToUpgrade, Payload);

	//Decrease by one the number of Attribute point (as it is spent just above)
	if(GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityClassInfo(GetAvatarActor());
	if(AbilityInfo != nullptr)
	{
		for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
		{
			if(!Info.AbilityTag.IsValid() || Info.LevelRequirement > Level) continue;

			if(GetSpecFromAbilityTag(Info.AbilityTag) == nullptr) //we've stumbled on an ability that we don't have in our ASC
			{
				//TODO What if we're level > Info.LevelRequirement && the ability is already unlocked??? does it bring it back to eligible status? 
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
				AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
				GiveAbility(AbilitySpec);
				MarkAbilitySpecDirty(AbilitySpec); //Force an ability spec to be replicated now instead of at the next update
				ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, AbilitySpec.Level);
			}
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if(IsValid(AbilitySpec.Ability))
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			//Assuming the ability only has one tag here
			if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if(IsValid(AbilitySpec.Ability))
	{
		for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
		{
			if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for(FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	//Locking the ability list, so we don't run into complications with abilities being added or removed while performing our search
	FScopedAbilityListLock ScopedAbilityListLock(*this);
	
	for (FGameplayAbilitySpec& ActivatableAbilitySpec : GetActivatableAbilities())
	{
		for(auto Tag : ActivatableAbilitySpec.Ability.Get()->AbilityTags)
		{
			if(Tag.MatchesTag(AbilityTag))
			{
				return &ActivatableAbilitySpec;
			}
		}
	}
	return nullptr;
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	//Broadcast for the widget controller for spell tree update
 	OnAbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& SelectedAbilityTag)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FGameplayAbilitySpec* SelectedAbilitySpec = GetSpecFromAbilityTag(SelectedAbilityTag);

	//If we don't make it inside the next if, the ability doesn't exist
	if(SelectedAbilitySpec)
	{
		//Updating the ability status
		FGameplayTag SelectedAbilityStatus = GetAbilityStatusFromSpec(*SelectedAbilitySpec);
		if(SelectedAbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Locked)) return;
	
		if(SelectedAbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			SelectedAbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
			SelectedAbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
		}
		else if(SelectedAbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || SelectedAbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			//Increasing Ability level like this doesn't cancel the ability if active. If we want to cancel it, we should remove the ability & give it back
			++SelectedAbilitySpec->Level;
		}
		
		//Spending a Spell Point
		if(GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		//Broadcast to the spell menu widget controller
		ClientUpdateAbilityStatus(SelectedAbilityTag, GetAbilityStatusFromSpec(*SelectedAbilitySpec), SelectedAbilitySpec->Level);

		//Since we've changed the abiliy spec, if we want to force its replication instead of waiting the next update
		MarkAbilitySpecDirty(*SelectedAbilitySpec);
	}
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if(!bAreStartupAbilitiesGiven)
	{
		bAreStartupAbilitiesGiven = true;
		OnAbilitiesGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClientOnGameplayEffectApplied_Implementation(UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayEffectSpec& InGameplayEffectSpec, FActiveGameplayEffectHandle InActiveGameplayEffectHandle)
{
	FGameplayTagContainer GameplayTagContainer;
	InGameplayEffectSpec.GetAllAssetTags(GameplayTagContainer);

	OnEffectAssetTagsDelegate.Broadcast(GameplayTagContainer);
}
