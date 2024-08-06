// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/ExecutionCalculation/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Datas/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(GetDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().BlockingChanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitDamagesDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitResistanceDef);
	
	RelevantAttributesToCapture.Add(GetDamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().PhysicalResistanceDef);
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluateParameters, const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToCaptureDefs) const
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	//Determines whether there is a debuff
	for (const TTuple<FGameplayTag, FGameplayTag>& DamageTypeDebuffPair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType = DamageTypeDebuffPair.Key;
		const FGameplayTag& DebuffType = DamageTypeDebuffPair.Value;
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.0f);

		if(TypeDamage > -0.5f) //Padding to -0.5f for floating point imprecision
		{
			//Determine if there was a successful debuff
			const float SourceChanceDebuff = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chances, false, -1.0f);
			
			float TargetDebuffResistance = 0.0f;
			const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageType];
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InTagsToCaptureDefs[ResistanceTag], EvaluateParameters, TargetDebuffResistance);
			TargetDebuffResistance = FMath::Max(TargetDebuffResistance, 0.0f);

			const float EffectiveDebuffChance = SourceChanceDebuff * (100 - TargetDebuffResistance ) / 100; //each point in our resistance takes off 1% of the debuff chance
			const bool bDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;
			if(bDebuff)
			{
				//TODO : find what to do when debuffing here
			}
		}
	}
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//Boilerplate steps
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = IsValid(SourceASC) ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = IsValid(TargetASC) ? TargetASC->GetAvatarActor() : nullptr;

	const int32 SourceAvatarLevel = SourceAvatar->Implements<UCombatInterface>() ? ICombatInterface::Execute_GetCharacterLevel(SourceAvatar) : 1;
	const int32 TargetAvatarLevel = TargetAvatar->Implements<UCombatInterface>() ? ICombatInterface::Execute_GetCharacterLevel(TargetAvatar) : 1;


	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		
	TagsToCaptureDefs.Empty();
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_Armor, GetDamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_BlockingChances, GetDamageStatics().BlockingChanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetDamageStatics().ArmorPenetrationDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitChances, GetDamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetDamageStatics().CriticalHitResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Secondary_CriticalHitDamages, GetDamageStatics().CriticalHitDamagesDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Arcane, GetDamageStatics().ArcaneResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Fire, GetDamageStatics().FireResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Lightning, GetDamageStatics().LightningResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Physical, GetDamageStatics().PhysicalResistanceDef);
	
	//Debuff
	DetermineDebuff(ExecutionParams, Spec, EvaluateParameters, TagsToCaptureDefs);

	
	//Get Damage Set by Caller
	float Damage = 0.0f;
	for (const TTuple<FGameplayTag, FGameplayTag>& GameplayTagPair : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = GameplayTagPair.Key;
		const FGameplayTag ResistanceTag = GameplayTagPair.Value;

		checkf(TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCaptureDefs[ResistanceTag];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);

		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		DamageTypeValue *= ( 100.f - Resistance ) / 100.f;
		
		Damage += DamageTypeValue;
	}
	
	//Capture Block chance on Target
	float TargetBlockChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().BlockingChanceDef, EvaluateParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.0f);

	bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetBlockedHit(EffectContextHandle, bBlocked);
	
	// If successful block, Halve the damage.
	Damage = bBlocked ? Damage /= 2.0f : Damage;

	//Target Armor Capture
	float TargetArmor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.0f);

	//Target Penetration Capture
	float SourceArmorPenetration = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.0f);

	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficient.Get()->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceAvatarLevel);

	const FRealCurve* ArmorCurve = CharacterClassInfo->DamageCalculationCoefficient.Get()->FindCurve(FName("Armor"), FString());
	const float TargetArmorCoefficient = ArmorCurve->Eval(TargetAvatarLevel);
	
	//Armor Penetration ignores a percentage of the Target's Armor
	//SourceArmorPenetration * 0.25 is used so it would take 400 armor to ignore 100%. It will take 4 Armor Penetration point to ignore 1% of Armor
	const float EffectiveArmor = TargetArmor * (100.0f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.0f;
	Damage *= (100.0f - EffectiveArmor * TargetArmorCoefficient) / 100.0f;

	//Source CriticalHitChance Capture
	float SourceCriticalHitChance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitChanceDef, EvaluateParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max(0.0f, SourceCriticalHitChance);

	//Source CriticalHitDamage Capture
	float SourceCriticalHitDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitDamagesDef, EvaluateParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max(0.0f, SourceCriticalHitDamage);
	
	//Target CriticalHitResistance Capture
	float TargetCriticalHitResistance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().CriticalHitResistanceDef, EvaluateParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max(0.0f, TargetCriticalHitResistance);

	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficient.Get()->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetAvatarLevel);

	//Critical Hit Resistance reduces Critical Hit Chances by a certain percentage
	const float EffectiveCriticalHitChances = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;	
	const bool bIsCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChances;

	UAuraAbilitySystemLibrary::SetCriticalHit(EffectContextHandle, bIsCriticalHit);
	Damage = bIsCriticalHit ? 2.0f * Damage + SourceCriticalHitDamage : Damage;
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
