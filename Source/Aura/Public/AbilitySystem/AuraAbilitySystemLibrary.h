// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Datas/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary | WidgetController")
	static class UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextContext);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary | WidgetController")
	static class UAttributesMenuWidgetController* GetAttributesMenuWidgetController(const UObject* WorldContextObject);

	//Initialize based Attributes based on the Character class and the level
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary | Character Class Defaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextContext, ECharacterClass CharacterClass, int32 Level, class UAbilitySystemComponent* AbilitySystemComponent);

	//Initialize based Attributes based on the Character class and the level
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary | Character Class Defaults")
	static void GiveStartupAbilities(const UObject* WorldContextContext, class UAbilitySystemComponent* AbilitySystemComponent);

};
