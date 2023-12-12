// Copyright Louis Pougis, All Rights Reserved.


#include "Inputs/AuraEnhancedInputComponent.h"

template <class UserClass, typename PressedFunctionType, typename ReleasedFunctionType, typename HeldFunctionType>
void UAuraEnhancedInputComponent::BindAbilityAction(const UAuraInputConfig* AuraInputConfig, UserClass* Object,
	PressedFunctionType PressedFunction, ReleasedFunctionType ReleasedFunction, HeldFunctionType HeldFunction)
{
	check(AuraInputConfig);

	for (const FAuraInputAction& Action : AuraInputConfig->AbilityInputAction)
	{
		if(IsValid(Action.InputAction) && Action.GameplayTag.IsValid())
		{
			if(PressedFunction)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunction, Action.GameplayTag);
			}

			if(ReleasedFunction)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunction, Action.GameplayTag);
			}
			
			if(HeldFunction)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunction, Action.GameplayTag);
			}
		}
	}
}