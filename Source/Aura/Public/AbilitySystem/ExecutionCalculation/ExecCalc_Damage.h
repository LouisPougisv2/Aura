// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "ExecCalc_Damage.generated.h"


//Raw internal struct used in C++ here
struct AuraDamageStatics
{
	//Following line is used to declare an FProperty && a Gameplay attribute capture def.
	//Note : "Armor" doesn't need to be a variable, it is only a token
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockingChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamages);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockingChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamages, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
	}

	void InitializeTagsToCaptureDefsMap()
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		
		TagsToCaptureDefs.Empty();
		TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_BlockingChances, BlockingChanceDef);
		TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitChances, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitDamages, CriticalHitDamagesDef);
		TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Physical, PhysicalResistanceDef);
	}
};

static AuraDamageStatics& GetDamageStatics()
{
	static AuraDamageStatics DamageStatics;
	return DamageStatics;
}

static void InitializeStaticTagsToCaptureDefsMap()
{
	GetDamageStatics().InitializeTagsToCaptureDefsMap();
}

/**
 * 
 */
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UExecCalc_Damage();

	//This is where all the calculation is going to be done (It is the heart of the Execution calculation)
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
