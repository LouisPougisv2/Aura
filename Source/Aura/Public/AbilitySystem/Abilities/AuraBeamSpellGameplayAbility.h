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

	//Function that traces the location of any enemy that would stand between the Player & the Beam Target Location.
	//If FirstTarget exist, it should become the hit actor & receive damages accordingly
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);
};
