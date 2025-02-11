// Copyright Louis Pougis, All Rights Reserved.


#include "UI/HUD/AuraHUD.h"

#include "UI/WidgetController/AttributesMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/SpellMenuAuraWidgetController.h"
#include "UI/Widgets/AuraUserWidget.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParam& WidgetControllerParam)
{
	if(OverlayWidgetController == nullptr)
	{
		ensureAlways(IsValid(OverlayWidgetControllerClass));
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParam);
		OverlayWidgetController->BindCallbacksToDependencies();
	}

	return OverlayWidgetController;
}

UAttributesMenuWidgetController* AAuraHUD::GetAttributesMenuWidgetController(const FWidgetControllerParam& WidgetControllerParam)
{
	if(AttributesMenuWidgetController == nullptr)
	{
		ensureAlways(IsValid(AttributesMenuWidgetControllerClass));
		AttributesMenuWidgetController = NewObject<UAttributesMenuWidgetController>(this, AttributesMenuWidgetControllerClass);
		AttributesMenuWidgetController->SetWidgetControllerParams(WidgetControllerParam);
		AttributesMenuWidgetController->BindCallbacksToDependencies();
	}
	
	return AttributesMenuWidgetController;
}

USpellMenuAuraWidgetController* AAuraHUD::GetSpellMenuWidgetController(const FWidgetControllerParam& WidgetControllerParam)
{
	if(SpellMenuWidgetController == nullptr)
	{
		ensureAlways(IsValid(SpellMenuWidgetControllerClass));
		SpellMenuWidgetController = NewObject<USpellMenuAuraWidgetController>(this, SpellMenuWidgetControllerClass);
		SpellMenuWidgetController->SetWidgetControllerParams(WidgetControllerParam);
		SpellMenuWidgetController->BindCallbacksToDependencies();
	}
	
	return SpellMenuWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	if(!ensureAlways(IsValid(OverlayWidgetClass))) UE_LOG(LogTemp, Error, TEXT("OverlayWidgetClass not initialized in BP, please fill out in BP_AuraHUD"));
	if(!ensureAlways(IsValid(OverlayWidgetControllerClass))) UE_LOG(LogTemp, Error, TEXT("OverlayWidgetControllerClass not initialized in BP, please fill out in BP_AuraHUD"));
	
	UUserWidget* UserWidget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(UserWidget);

	const FWidgetControllerParam WidgetControllerParam(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParam);
	
	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	
	UserWidget->AddToViewport();

	
}
