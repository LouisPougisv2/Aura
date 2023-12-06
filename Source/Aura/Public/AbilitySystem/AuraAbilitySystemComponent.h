// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTagsDelegate, const FGameplayTagContainer& /* Asset Tags */)
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

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	//Public because our widget controller will need to bind to it
	FEffectAssetTagsDelegate OnEffectAssetTagsDelegate;
	
protected:

	void OnGameplayEffectApplied(UAbilitySystemComponent* InAbilitySystemComponent, const FGameplayEffectSpec& InGameplayEffectSpec, FActiveGameplayEffectHandle InActiveGameplayEffectHandle);
	
};
