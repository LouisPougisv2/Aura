// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TObjectPtr<class UAuraUserWidget> OverlayWidget;

	UFUNCTION()
	class UOverlayWidgetController* GetOverlayWidgetController(const struct FWidgetControllerParam& WidgetControllerParam);

	UFUNCTION()
	class UAttributesMenuWidgetController* GetAttributesMenuWidgetController(const struct FWidgetControllerParam& WidgetControllerParam);

	void InitOverlay(APlayerController* PC, APlayerState* PS, class UAbilitySystemComponent* ASC, class UAttributeSet* AS);

private:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributesMenuWidgetController> AttributesMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributesMenuWidgetController> AttributesMenuWidgetControllerClass;
};
