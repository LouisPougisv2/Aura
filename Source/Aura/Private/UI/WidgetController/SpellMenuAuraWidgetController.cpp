// Copyright Louis Pougis, All Rights Reserved.


#include "UI/WidgetController/SpellMenuAuraWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Datas/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuAuraWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	BroadcastAbilityInfo();
	CurrentSpellPoints = GetAuraPlayerState()->GetPlayerSpellPoints();
	OnPlayerSpellPointChanged.Broadcast(GetAuraPlayerState()->GetPlayerSpellPoints());
}

void USpellMenuAuraWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	GetAuraAbilitySystemComponent()->OnAbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
		if(CurrentSelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
		{
			CurrentSelectedAbility.AbilityStatus = StatusTag;
			UpdateSelectedAbilityUI(StatusTag, CurrentSpellPoints);
		}
		
		if(AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoFromTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetAuraPlayerState()->OnSpellPointsChangedDelegate.AddLambda([this](int32 SpellPoint)
	{
		CurrentSpellPoints = SpellPoint;
		UpdateSelectedAbilityUI(CurrentSelectedAbility.AbilityStatus, CurrentSpellPoints);
		
		OnPlayerSpellPointChanged.Broadcast(SpellPoint);
	});
}

void USpellMenuAuraWidgetController::OnSpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	FGameplayTag AbilityStatus;
	const int32 AvailableSpellPoints = GetAuraPlayerState()->GetPlayerSpellPoints();
	
	const bool IsAbilityTagValid = AbilityTag.IsValid();
	const bool IsTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	const bool bIsSpecValid = GetAuraAbilitySystemComponent()->GetSpecFromAbilityTag(AbilityTag) != nullptr;
	
	//AbilityTag that we don't have yet (locked status) or an ability tag that doesn't exist (wrong tag)
	if(!IsAbilityTagValid || IsTagNone || !bIsSpecValid)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = UAuraAbilitySystemComponent::GetAbilityStatusFromSpec(*GetAuraAbilitySystemComponent()->GetSpecFromAbilityTag(AbilityTag));
	}

	CurrentSelectedAbility.AbilityTag = AbilityTag;
	CurrentSelectedAbility.AbilityStatus = AbilityStatus;

	UpdateSelectedAbilityUI(AbilityStatus, AvailableSpellPoints);
}

void USpellMenuAuraWidgetController::EnableSpellPointsAndEquipButtons(const FGameplayTag& AbilityStatus, const int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	if(AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnableSpellPointsButton = SpellPoints > 0;
		bShouldEnableEquipButton = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		bShouldEnableSpellPointsButton = SpellPoints > 0;
	}
}

void USpellMenuAuraWidgetController::UpdateSelectedAbilityUI(const FGameplayTag& AbilityStatus, const int32 SpellPoints)
{
	bool bShouldEnableSpellPointsButton = false;
	bool bShouldEnableEquipButton = false;
	EnableSpellPointsAndEquipButtons(AbilityStatus, SpellPoints, bShouldEnableSpellPointsButton, bShouldEnableEquipButton);
	OnSpellGlobeSelectedDelegate.Broadcast(bShouldEnableSpellPointsButton, bShouldEnableEquipButton);
}
