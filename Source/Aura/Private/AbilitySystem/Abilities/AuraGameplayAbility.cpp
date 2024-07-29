// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

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
