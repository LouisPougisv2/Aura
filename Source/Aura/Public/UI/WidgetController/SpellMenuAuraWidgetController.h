// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuAuraWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSpellGlobeSelectedSignature, bool ,bShouldEnableSpellGlobeButton, bool ,bShouldEnableEquipButton, FString, SpellDescription, FString, NextLevelSpellDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellGlobeReassignedSignature, const FGameplayTag&, AbilityTag);
struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag AbilityStatus = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuAuraWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature OnPlayerSpellPointChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnSpellGlobeSelectedSignature OnSpellGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnWaitForEquipSelectionSignature OnWaitForEquipSelectionDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnWaitForEquipSelectionSignature OnStopWaitingForEquipSelectionDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnSpellGlobeReassignedSignature OnSpellGlobeReassignedDelegate;

	UFUNCTION(BlueprintCallable)
	void OnSpellGlobeSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonsPressed();

	UFUNCTION(BlueprintCallable)
	void DeselectGlobe();

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnSpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& AbilityStatus, const FGameplayTag& NewSlot, const FGameplayTag& OldSlot);

private:

	void EnableSpellPointsAndEquipButtons(const FGameplayTag& AbilityStatus, const int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton);

	void UpdateSelectedAbilityUI(const FGameplayTag& AbilityStatus, const int32 SpellPoints, const FGameplayTag& AbilityTag);
	
	FSelectedAbility CurrentSelectedAbility = FSelectedAbility{FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked};
	int32 CurrentSpellPoints;
	bool bIsWaitingForEquipSelection = false;

	FGameplayTag CurrentSelectedSlot = FGameplayTag();
};
