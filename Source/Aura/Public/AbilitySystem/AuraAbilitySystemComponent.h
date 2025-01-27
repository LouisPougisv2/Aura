// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTagsDelegate, const FGameplayTagContainer& /* Asset Tags */)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChangedDelegate, const FGameplayTag& /*Ability Tag*/, const FGameplayTag& /*Ability Status*/, int32 /*AbilityLevel*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnAbilityEquippedSignature, const FGameplayTag& /*Ability Tag*/, const FGameplayTag& /*New Ability Status*/, const FGameplayTag& /* New Slot*/, const FGameplayTag& /*Previous Slot*/);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	void OnAbilityInfoSet();
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartUpPassiveAbilities);

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTagToUpgrade);

	void UpdateAbilityStatuses(int32 Level);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static bool HasAbilityASlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot);
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	bool GetAbilityDescriptionsFromAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription, const class UAbilityInfo& AbilityInfo);
	
	//Public because our widget controller will need to bind to it
	FEffectAssetTagsDelegate OnEffectAssetTagsDelegate;

	//Broadcast when we give our abilities
	FAbilitiesGiven OnAbilitiesGivenDelegate;

	FAbilityStatusChangedDelegate OnAbilityStatusChangedDelegate;

	bool bAreStartupAbilitiesGiven = false;

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& SelectedAbilityTag);

	/**
	* Equip Spell
	**/
	
	FOnAbilityEquippedSignature OnAbilityEquippedDelegate;
	
	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag);

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& NewAbilityTag, const FGameplayTag& NewAbilityStatus, const FGameplayTag& PreviousSlotTag, const FGameplayTag& NewSlotTag);
	
	void ClearSlot(FGameplayAbilitySpec* Spec);
	void ClearAbilitiesFromSlot(const FGameplayTag& Slot);

	/**
	* END Equip Spell
	**/
	
protected:

	UFUNCTION(Client, Reliable)
	void ClientOnGameplayEffectApplied(UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayEffectSpec& InGameplayEffectSpec, FActiveGameplayEffectHandle InActiveGameplayEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);
	
	virtual void OnRep_ActivateAbilities() override;
};
