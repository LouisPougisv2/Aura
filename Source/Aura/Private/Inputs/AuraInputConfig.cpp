// Copyright Louis Pougis, All Rights Reserved.


#include "Inputs/AuraInputConfig.h"

#include "InputAction.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& GameplayTag, bool bLogNotFound) const
{
	for (const FAuraInputAction& AuraInputAction : AbilityInputAction)
	{
		if(AuraInputAction.GameplayTag.MatchesTagExact(GameplayTag) && IsValid(AuraInputAction.InputAction))
		{
			return AuraInputAction.InputAction;
		}
	}
	if(bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Input Action corresponding to Attribute with Tag [%s] not found on InputConfig [%s]"), *GameplayTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
