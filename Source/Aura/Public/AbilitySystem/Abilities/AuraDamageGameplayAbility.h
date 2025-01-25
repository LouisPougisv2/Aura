// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void CauseDamages(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	struct FDamageEffectParams MakeDamageEffectParamsFromClassDefault(AActor* TargetActor = nullptr) const;
	
protected:

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage;

	//Could be a FScalableFloat with CT based on Level
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffChances = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDamage = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffFrequency = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DeathImpulseMagnitude = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockbackMagnitude = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockbackChance = 0.0f;
	
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& InTaggedMontages) const;

	float GetDamageAtLevel(int32 Level) const;
};
