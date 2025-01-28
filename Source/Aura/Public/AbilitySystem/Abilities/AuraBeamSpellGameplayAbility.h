// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpellGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpellGameplayAbility : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadWrite, Category = "Beam Options")
	FVector MouseHitLocation = FVector();

	UPROPERTY(BlueprintReadWrite, Category = "Beam Options")
	TObjectPtr<AActor> MouseHitActor = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Beam Options")
	TObjectPtr<APlayerController> OwnerPlayerController = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Beam Options")
	TObjectPtr<ACharacter> OwnerCharacter = nullptr;

public:

	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& MouseHitResult);

	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();
};
