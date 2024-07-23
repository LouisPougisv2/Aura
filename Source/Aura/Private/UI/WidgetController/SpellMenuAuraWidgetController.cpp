// Copyright Louis Pougis, All Rights Reserved.


#include "UI/WidgetController/SpellMenuAuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Datas/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuAuraWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	BroadcastAbilityInfo();
	OnPlayerSpellPointChanged.Broadcast(GetAuraPlayerState()->GetPlayerSpellPoints());
}

void USpellMenuAuraWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	GetAuraAbilitySystemComponent()->OnAbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
		if(AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoFromTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetAuraPlayerState()->OnSpellPointsChangedDelegate.AddLambda([this](int32 SpellPoint)
	{
		OnPlayerSpellPointChanged.Broadcast(SpellPoint);
	});
}
