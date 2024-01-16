// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	bool IsHitBlocked() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }

	void SetIsBlockedHit(bool IsBlocked) { bIsBlockedHit = IsBlocked; }
	void SetIsCriticalHit(bool IsCritical) { bIsCriticalHit = IsCritical; }
	
	///** Returns the actual struct used for serialization */
	virtual UScriptStruct* GetScriptStruct() const override;

	/** Custom serialization */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	
protected:

	UPROPERTY()
	bool bIsBlockedHit = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;
};