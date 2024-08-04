// Copyright Louis Pougis, All Rights Reserved.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Datas/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAttributeSet()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAttributeSet()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAttributeSet()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAttributeSet()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPlayerState()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPlayerState()->OnLevelChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnLevelChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetHealthAttribute())
	.AddLambda( [this](const FOnAttributeChangeData& Data){ OnHealthChanged.Broadcast(Data.NewValue) ;});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetMaxHealthAttribute())
	.AddLambda( [this](const FOnAttributeChangeData& Data){ OnMaxHealthChanged.Broadcast(Data.NewValue) ;});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetManaAttribute())
	.AddLambda( [this](const FOnAttributeChangeData& Data){ OnManaChanged.Broadcast(Data.NewValue) ;});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetMaxManaAttribute())
	.AddLambda( [this](const FOnAttributeChangeData& Data){ OnMaxManaChanged.Broadcast(Data.NewValue) ;});
	
	if(IsValid(GetAuraAbilitySystemComponent()))
	{
		GetAuraAbilitySystemComponent()->OnAbilityEquippedDelegate.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);

		if(GetAuraAbilitySystemComponent()->bAreStartupAbilitiesGiven) //Here we know that our Startup Abilities have been given!
		{
			BroadcastAbilityInfo();
		}
		else
		{
			GetAuraAbilitySystemComponent()->OnAbilitiesGivenDelegate.AddUObject(this, &UAuraWidgetController::BroadcastAbilityInfo);
		}
		
		GetAuraAbilitySystemComponent()->OnEffectAssetTagsDelegate.AddLambda( [this] (const FGameplayTagContainer& GameplayTagContainer)
		{
			for (const FGameplayTag& Tag : GameplayTagContainer)
			{
				if(GEngine)
				{
					// For example, say that Tag = Message.HealthPotion
					// "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will 
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if(Tag.MatchesTag(MessageTag))
					{
                        FUIWidgetRow* FoundRow = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
                        OnMessageWidgetRow.Broadcast(*FoundRow);
					}
				}
			}
		});
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetAuraPlayerState()->PlayerLevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to check LevelUpInfo, please fill out AuraPlayerState Blueprint"));

	const int32 Level = LevelUpInfo->FindLevelForExp(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformations.Num();

	if(Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformations[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformations[Level - 1].LevelUpRequirement;

		//Next lime allows us to know what to divide our current exp by 
		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

		OnXPPercentChanged.Broadcast(XPBarPercent);
	}
}

void UOverlayWidgetController::OnLevelChanged(int32 NewLevel)
{
	OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& AbilityStatus, const FGameplayTag& NewSlot, const FGameplayTag& OldSlot)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = OldSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;

	//Broadcast empty info if previous slot is a valid slot. Only if Equipping an already-equipped spell
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo NewSlotInfo = AbilityInfo->FindAbilityInfoFromTag(AbilityTag);
	NewSlotInfo.StatusTag = AbilityStatus;
	NewSlotInfo.InputTag = NewSlot;

	//Broadcast new slot
	AbilityInfoDelegate.Broadcast(NewSlotInfo);
}

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& GameplayTag)
{
	return DataTable->FindRow<T>(GameplayTag.GetTagName(), TEXT(""));
}
