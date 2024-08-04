// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s</>, <Level>%d</>"), L"Default Ability Name - LoremIpsum LoremIpsum LoremIpsum", Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 NextLevel)
{
	return FString::Printf(TEXT("<Default>Next Level: </>, <Level>%d</> \n<Default>Causes way more damages</>"), NextLevel);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked until level : %d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(int32 Level) const
{
	float ManaCost = 0.0f;
	if(const UGameplayEffect* CostGameplayEffect = GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo ModifierInfo : CostGameplayEffect->Modifiers)
		{
			if(ModifierInfo.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				ModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(Level, ManaCost);
				break;
			}
		}
	}
	return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(int32 Level) const
{
	float Cooldown = 0.0f;
	if(UGameplayEffect* GameplayEffectCooldown = GetCooldownGameplayEffect())
	{
		GameplayEffectCooldown->DurationMagnitude.GetStaticMagnitudeIfPossible(Level, Cooldown);
	}
	return Cooldown;
}
