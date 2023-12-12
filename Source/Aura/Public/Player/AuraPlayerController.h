// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/EnemyInterface.h"
#include "AuraPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AAuraPlayerController();
	
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere, Category = "Enhanced Inputs")
	TObjectPtr<class UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category = "Enhanced Inputs")
	TObjectPtr<class UInputAction> MoveInputAction;

	UPROPERTY(EditAnywhere, Category = "Enhanced Inputs")
	TObjectPtr<class UAuraInputConfig> AuraInputConfig;

	UPROPERTY()
	TObjectPtr<class UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	IEnemyInterface* LastActor;
	IEnemyInterface* CurrentActor;
	FHitResult CursorHit;
	
	void Move(const struct FInputActionValue& InputActionValue);
	void CursorTrace();

	//Input Callback functions
	void AbilityInputTagPressed(struct FGameplayTag InputTag);
	void AbilityInputTagReleased(struct FGameplayTag InputTag);
	void AbilityInputTagHeld(struct FGameplayTag InputTag);

	//-------------- Click To Move Variable --------------
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();

	void AutoRunning();

	FVector CachedDestination = FVector::ZeroVector;

	float FollowCursorTime = 0.0f;
	float ShortPressedThreshold = 0.5f;
	bool bIsAutoRunning = false;
	bool bIsTargetingEnemy = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.0f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USplineComponent> SplinePath;
	//-------------- End of Click To Move Variable --------------
};
