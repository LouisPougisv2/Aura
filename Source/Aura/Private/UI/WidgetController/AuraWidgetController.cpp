// Copyright Louis Pougis, All Rights Reserved.


#include "UI/WidgetController/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParam& WidgetControllerParam)
{
	PlayerController = WidgetControllerParam.PlayerController;
	PlayerState = WidgetControllerParam.PlayerState;
	AbilitySystemComponent = WidgetControllerParam.AbilitySystemComponent;
	AttributeSet = WidgetControllerParam.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
	
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
	
}
