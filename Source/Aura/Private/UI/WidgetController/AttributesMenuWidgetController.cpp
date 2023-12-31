// Copyright Louis Pougis, All Rights Reserved.


#include "UI/WidgetController/AttributesMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Datas/UAttributeInfo.h"

void UAttributesMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	
	for (auto& Tag : AuraAttributeSet->TagsToAttributesMap)
	{
		BroadcastAttributeInfo(Tag.Key, Tag.Value());
	}
}

void UAttributesMenuWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	
	for (auto& Tag : AuraAttributeSet->TagsToAttributesMap)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Tag.Value()).AddLambda
			(
				[this, Tag](const FOnAttributeChangeData& Data)
				{
					BroadcastAttributeInfo(Tag.Key, Tag.Value());
				}
		);
	}
}

void UAttributesMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& GameplayAttribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag, true);
	Info.AttributeValue = GameplayAttribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
