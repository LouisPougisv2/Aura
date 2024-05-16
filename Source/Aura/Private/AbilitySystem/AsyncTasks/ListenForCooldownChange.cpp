// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/AsyncTasks/ListenForCooldownChange.h"

#include "AbilitySystemComponent.h"

UListenForCooldownChange* UListenForCooldownChange::ListenForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
	UListenForCooldownChange* ListenForCooldownChange = NewObject<UListenForCooldownChange>();
	ListenForCooldownChange->ASC = AbilitySystemComponent;
	ListenForCooldownChange->CoolDownTag = InCooldownTag;
	
	if(!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		ListenForCooldownChange->OnEndTask();
		return nullptr;
	}

	//To know when a cooldown has ended (Cooldown Tag removed)
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(ListenForCooldownChange, &UListenForCooldownChange::OnCooldownTagChanged);

	//To know when an effect cooldown has been applied
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(ListenForCooldownChange, &UListenForCooldownChange::OnActiveEffectAdded);
	
	return ListenForCooldownChange;
}

void UListenForCooldownChange::OnEndTask()
{
	if(!IsValid(ASC)) return;
	
	//Removing the callback from its delegate
	ASC->RegisterGameplayTagEvent(CoolDownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UListenForCooldownChange::OnCooldownTagChanged(const FGameplayTag InCoolDownTag, int32 NewCount)
{
	if(NewCount == 0)
	{
		CooldownEndDelegate.Broadcast(0.0f);
	}
}

void UListenForCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	if(AssetTags.HasTagExact(CoolDownTag) || GrantedTags.HasTagExact(CoolDownTag))
	{
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CoolDownTag.GetSingleTagContainer());
		
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		if(TimesRemaining.Num() > 0)
		{
			int32 index = 0;
			float TimeRemaining = TimesRemaining[0];
			
			for (float Time : TimesRemaining)
			{
				if(Time > TimeRemaining) TimeRemaining = Time;
			}
			CooldownStartDelegate.Broadcast(TimeRemaining);
		}
	}
}
