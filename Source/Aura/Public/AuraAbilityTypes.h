﻿// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"


USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}

	//Because we're planning on passing this struct to AuraAbilitySystemLibrary
	UPROPERTY()
	TObjectPtr<UObject> WorldContext = nullptr;

	UPROPERTY()
	TSubclassOf<class UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;

	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY()
	float BaseDamage = 0.0f;

	UPROPERTY()
	float AbilityLevel = 1.0f;

	UPROPERTY()
	float DebuffChances = 0.0f;

	UPROPERTY()
	float DebuffDamage = 0.0f;

	UPROPERTY()
	float DebuffFrequency = 0.0f;

	UPROPERTY()
	float DebuffDuration = 0.0f;

	UPROPERTY()
	float DeathImpulseMagnitude = 0.0f;

	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;
	
	UPROPERTY()
	float KnockbackMagnitude = 0.0f;
	
	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;

	UPROPERTY()
	float KnockbackChance = 0.0f;
};
USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	bool IsHitBlocked() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsDebuffSuccessful() const { return bIsDebuffSuccessful; }
	float GetDebuffDamage() const { return DebuffDamage; }
	float GetDebuffFrequency() const { return DebuffFrequency; }
	float GetDebuffDuration() const { return DebuffDuration; }
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }
	FVector GetDeathImpulse() const { return DeathImpulse; }
	float GetKnockBackMagnitude() const { return KnockbackMagnitude; }
	FVector GetKnockBackForce() const { return KnockbackForce; }
	float GetKnockBackChance() const { return KnockbackChance; }

	void SetIsBlockedHit(bool IsBlocked) { bIsBlockedHit = IsBlocked; }
	void SetIsCriticalHit(bool IsCritical) { bIsCriticalHit = IsCritical; }
	void SetIsDebuffSuccessful(bool InIsDebuffSuccessful) { bIsDebuffSuccessful = InIsDebuffSuccessful; }
	void SetDebuffDamage(float InDebuffDamage) { DebuffDamage = InDebuffDamage; }
	void SetDebuffFrequency(float InDebuffFrequency) { DebuffFrequency = InDebuffFrequency; }
	void SetDebuffDuration(float InDebuffDuration) { DebuffDuration = InDebuffDuration; }
	void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }
	void SetDeathImpulse(const FVector& InDeathImpulse) { DeathImpulse = InDeathImpulse; }
	void SetKnockBackMagnitude(float InKnockBackMagnitude) { KnockbackMagnitude = InKnockBackMagnitude; }
	void SetKnockBackForce(const FVector& InForce) { KnockbackForce = InForce; }
	void SetKnockBackChances(float InChances) { KnockbackChance = InChances; }
	
	///** Returns the actual struct used for serialization */
	virtual UScriptStruct* GetScriptStruct() const override;

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const override;

	/** Custom serialization */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	
protected:

	UPROPERTY()
	bool bIsBlockedHit = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	bool bIsDebuffSuccessful = false;

	UPROPERTY()
	float DebuffDamage = 0.0f;
	
	UPROPERTY()
	float DebuffDuration = 0.0f;

	UPROPERTY()
	float DebuffFrequency = 0.0f;

	//We don't give it a UPROPERTY() as it doesn't garbage collect the
	TSharedPtr<FGameplayTag> DamageType;

	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;

	UPROPERTY()
	float KnockbackMagnitude = 0.0f;
	
	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;

	UPROPERTY()
	float KnockbackChance = 0.0f;
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};