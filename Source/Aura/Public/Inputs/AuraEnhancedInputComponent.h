// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraEnhancedInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	template<class UserClass, typename PressedFunctionType, typename ReleasedFunctionType, typename HeldFunctionType>
	void BindAbilityAction(const UAuraInputConfig* AuraInputConfig, UserClass* Object, PressedFunctionType PressedFunction, ReleasedFunctionType ReleasedFunction, HeldFunctionType HeldFunction);
};
