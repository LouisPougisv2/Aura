// Copyright Louis Pougis, All Rights Reserved.


#include "UI/WidgetController/AttributesMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Datas/UAttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributesMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	
	for (auto& Tag : AuraAS->TagsToAttributesMap)
	{
		BroadcastAttributeInfo(Tag.Key, Tag.Value());
	}

	OnPlayerAttributePointChangedDelegate.Broadcast(GetAuraPlayerState()->GetPlayerAttributePoints());
	OnPlayerSpellPointsChangedDelegate.Broadcast(GetAuraPlayerState()->GetPlayerSpellPoints());
}

void UAttributesMenuWidgetController::BindCallbacksToDependencies()
{
	check(AttributeInfo);
	for (auto& Tag : GetAuraAttributeSet()->TagsToAttributesMap)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Tag.Value()).AddLambda
			(
				[this, Tag](const FOnAttributeChangeData& Data)
				{
					BroadcastAttributeInfo(Tag.Key, Tag.Value());
				}
		);
	}

	GetAuraPlayerState()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			OnPlayerAttributePointChangedDelegate.Broadcast(Points);
		});

	GetAuraPlayerState()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 SpellPoints)
		{
			OnPlayerSpellPointsChangedDelegate.Broadcast(SpellPoints);
		});
}

void UAttributesMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAuraAbilitySystemComponent()->UpgradeAttribute(AttributeTag);
}

void UAttributesMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& GameplayAttribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag, true);
	Info.AttributeValue = GameplayAttribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
