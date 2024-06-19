// Copyright Louis Pougis, All Rights Reserved.


#include "UI/WidgetController/AttributesMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Datas/UAttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributesMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	
	for (auto& Tag : AuraAttributeSet->TagsToAttributesMap)
	{
		BroadcastAttributeInfo(Tag.Key, Tag.Value());
	}

	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	OnPlayerAttributePointChangedDelegate.Broadcast(AuraPlayerState->GetPlayerAttributePoints());
	OnPlayerSpellPointsChangedDelegate.Broadcast(AuraPlayerState->GetPlayerSpellPoints());
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

	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			OnPlayerAttributePointChangedDelegate.Broadcast(Points);
		});

	AuraPlayerState->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 SpellPoints)
		{
			OnPlayerSpellPointsChangedDelegate.Broadcast(SpellPoints);
		});
}

void UAttributesMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	auto AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraAbilitySystemComponent->UpgradeAttribute(AttributeTag);
}

void UAttributesMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& GameplayAttribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag, true);
	Info.AttributeValue = GameplayAttribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
