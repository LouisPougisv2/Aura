// Copyright Louis Pougis, All Rights Reserved.


#include "UI/HUD/AuraHUD.h"

#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/Widgets/AuraUserWidget.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParam& WidgetControllerParam)
{
	if(OverlayWidgetController == nullptr)
	{
		ensureAlways(IsValid(OverlayWidgetControllerClass));
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParam);
	}

	return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	if(!ensureAlways(IsValid(OverlayWidgetClass))) UE_LOG(LogTemp, Error, TEXT("OverlayWidgetClass not initialized in BP, please fill out in BP_AuraHUD"));
	if(!ensureAlways(IsValid(OverlayWidgetControllerClass))) UE_LOG(LogTemp, Error, TEXT("OverlayWidgetControllerClass not initialized in BP, please fill out in BP_AuraHUD"));
	
	UUserWidget* UserWidget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(UserWidget);

	const FWidgetControllerParam WidgetControllerParam(PC, PS, ASC, AS);
	OverlayWidget->SetWidgetController(GetOverlayWidgetController(WidgetControllerParam));
	
	UserWidget->AddToViewport();

	
}
