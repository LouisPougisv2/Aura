// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/ModifierMagnitudeCalculations/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//Gather tags from source and targets
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	//In order to capture an attribute and get that attribute's magnitude, we need to create a FAggregatorEvaluateParameters
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//Getting the Backing Attribute's magnitude
	float Vigor = 0.0f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.0f);

	//Getting the Character's Level
	const int32 CharacterLevel = Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>() ? ICombatInterface::Execute_GetCharacterLevel(Spec.GetContext().GetSourceObject()) : 1;

	return 80.0f + (2.5 * Vigor) + (10 * CharacterLevel);
}
