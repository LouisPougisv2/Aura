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
	static class UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary | WidgetController")
	static class UAttributesMenuWidgetController* GetAttributesMenuWidgetController(const UObject* WorldContextObject);

	//Initialize based Attributes based on the Character class and the level
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary | Character Class Defaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 Level, class UAbilitySystemComponent* AbilitySystemComponent);

	//Initialize based Attributes based on the Character class and the level
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary | Character Class Defaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, class UAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary | Character Class Defaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary | GameplayEffect")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary | GameplayEffect")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	//The UPARAM(ref) allows us to have the EffectContextHandle ref as input pin. Without it, it would be an output pin (unreal default's behaviour for non const param)
	//The Input pin will have a diamond shape (not a circle one)
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary | GameplayEffect")
	static void SetBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bIsBlocked);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary | GameplayEffect")
	static void SetCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bIsCritical);
};
