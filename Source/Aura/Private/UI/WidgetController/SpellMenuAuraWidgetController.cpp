// Copyright Louis Pougis, All Rights Reserved.


#include "UI/WidgetController/SpellMenuAuraWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Datas/AbilityInfo.h"
#include "Player/AuraPlayerController.h"
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

	GetAuraAbilitySystemComponent()->OnAbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
	{
		if(CurrentSelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
		{
			CurrentSelectedAbility.AbilityStatus = StatusTag;
			UpdateSelectedAbilityUI(StatusTag, CurrentSpellPoints, AbilityTag);
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
		UpdateSelectedAbilityUI(CurrentSelectedAbility.AbilityStatus, CurrentSpellPoints, CurrentSelectedAbility.AbilityTag);
		
		OnPlayerSpellPointChanged.Broadcast(SpellPoint);
	});

	//Equipping Ability binding
	GetAuraAbilitySystemComponent()->OnAbilityEquippedDelegate.AddUObject(this, &USpellMenuAuraWidgetController::OnAbilityEquipped);
}

void USpellMenuAuraWidgetController::OnSpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if(bIsWaitingForEquipSelection)
	{
		const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoFromTag(AbilityTag).AbilityType;
		OnStopWaitingForEquipSelectionDelegate.Broadcast(AbilityType);
		bIsWaitingForEquipSelection = false;
	}
	
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

	UpdateSelectedAbilityUI(AbilityStatus, AvailableSpellPoints, CurrentSelectedAbility.AbilityTag);
}

void USpellMenuAuraWidgetController::SpendPointButtonsPressed()
{
	GetAuraAbilitySystemComponent()->ServerSpendSpellPoints(CurrentSelectedAbility.AbilityTag);
}

void USpellMenuAuraWidgetController::DeselectGlobe()
{
	if(bIsWaitingForEquipSelection)
	{
		const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoFromTag(CurrentSelectedAbility.AbilityTag).AbilityType;
		OnStopWaitingForEquipSelectionDelegate.Broadcast(AbilityType);
		bIsWaitingForEquipSelection = false;
	}
	
	CurrentSelectedAbility.AbilityTag = FAuraGameplayTags::Get().Abilities_None;
	CurrentSelectedAbility.AbilityStatus = FAuraGameplayTags::Get().Abilities_Status_Locked;
	
	OnSpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuAuraWidgetController::EquipButtonPressed()
{
	const FGameplayTag& CurrentSelectedAbilityType = AbilityInfo->FindAbilityInfoFromTag(CurrentSelectedAbility.AbilityTag).AbilityType;
	OnWaitForEquipSelectionDelegate.Broadcast(CurrentSelectedAbilityType);

	bIsWaitingForEquipSelection = true;

	//TODO verify if GetStatusFromAbilityTag() needs to be deleted
	//const FGameplayTag SelectedAbilityStatus = GetAuraAbilitySystemComponent()->GetStatusFromAbilityTag(CurrentSelectedAbility.AbilityTag);
	if(CurrentSelectedAbility.AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		const FGameplayAbilitySpec* CurrentSelectedAbilitySpec = GetAuraAbilitySystemComponent()->GetSpecFromAbilityTag(CurrentSelectedAbility.AbilityTag);
		const FGameplayTag SelectedAbilityInputTag = GetAuraAbilitySystemComponent()->GetInputTagFromSpec(*CurrentSelectedAbilitySpec);
		CurrentSelectedSlot = SelectedAbilityInputTag;
	}
}

void USpellMenuAuraWidgetController::OnSpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if(!bIsWaitingForEquipSelection) return;

	//check Selected Ability type against Slot tag (no offensive spell can be assigned to passive & vice-versa)
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoFromTag(CurrentSelectedAbility.AbilityTag).AbilityType;
	if(!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	//Assigning an ability to a new slot or reassigning it
	GetAuraAbilitySystemComponent()->ServerEquipAbility(CurrentSelectedAbility.AbilityTag, SlotTag);
	
}

void USpellMenuAuraWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& AbilityStatus, const FGameplayTag& NewSlot, const FGameplayTag& OldSlot)
{
	bIsWaitingForEquipSelection = false;

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

	OnStopWaitingForEquipSelectionDelegate.Broadcast(AbilityInfo->FindAbilityInfoFromTag(AbilityTag).AbilityType);
	OnSpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	DeselectGlobe();
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

void USpellMenuAuraWidgetController::UpdateSelectedAbilityUI(const FGameplayTag& AbilityStatus, const int32 SpellPoints, const FGameplayTag& AbilityTag)
{
	bool bShouldEnableSpellPointsButton = false;
	bool bShouldEnableEquipButton = false;
	EnableSpellPointsAndEquipButtons(AbilityStatus, SpellPoints, bShouldEnableSpellPointsButton, bShouldEnableEquipButton);

	FString AbilityDescription;
	FString NextLevelAbilityDescription;
	GetAuraAbilitySystemComponent()->GetAbilityDescriptionsFromAbilityTag(AbilityTag, AbilityDescription, NextLevelAbilityDescription, *AbilityInfo.Get());
	OnSpellGlobeSelectedDelegate.Broadcast(bShouldEnableSpellPointsButton, bShouldEnableEquipButton, AbilityDescription, NextLevelAbilityDescription);
}
