// Copyright Louis Pougis, All Rights Reserved.


#include "UI/WidgetController/AttributesMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Datas/UAttributeInfo.h"

void UAttributesMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	FAuraAttributeInfo Info;
	for (auto& Tag : AuraAttributeSet->TagsToAttributesMap)
	{
		Info = AttributeInfo->FindAttributeInfoForTag(Tag.Key, true);
		Info.AttributeValue = Tag.Value().GetNumericValue(AuraAttributeSet);
		AttributeInfoDelegate.Broadcast(Info);
	}
}

void UAttributesMenuWidgetController::BindCallbacksToDependencies()
{
	
}
