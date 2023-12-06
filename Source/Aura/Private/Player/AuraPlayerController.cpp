// Copyright Louis Pougis, All Rights Reserved.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraPlayerCharacter.h"
#include "Components/SplineComponent.h"
#include "Inputs/AuraEnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	
	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("SplinePath"));
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(MappingContext);

	//Binding mapping context to Player Controller
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(IsValid(LocalPlayerSubsystem))
	{
		LocalPlayerSubsystem->AddMappingContext(MappingContext, 0);
	}

	//---------------- Cursor settings ----------------
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Hand;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	//---------------- END Cursor settings ----------------
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraEnhancedInputComponent* AuraInputComponent = CastChecked<UAuraEnhancedInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	AuraInputComponent->BindAbilityAction(AuraInputConfig, this, &AAuraPlayerController::AbilityInputTagPressed, &AAuraPlayerController::AbilityInputTagReleased, &AAuraPlayerController::AbilityInputTagHeld);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if(!CursorHit.bBlockingHit) return;
	
	LastActor = CurrentActor;
	CurrentActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	/** Line trace from Cursor scenarios:
	*		A. LastActor & CurrentActor are both null
	*			-> Do Nothing
	*		B. LastActor is null but Current is valid
	*			-> Call HighLight function
	*		C. LastActor is Valid && Current is null
	*			-> Call UnHighlight function
	*		D. LastActor & CurrentActor are both valid but LastActor != CurrentActor
	*			-> Call UnHighlight for LastActor & Highlight for CurrentActor
	*		E. LastActor & CurrentActor are both valid ad are the same
	*			-> Do nothing
	**/

	if(LastActor != CurrentActor)
	{
		if(LastActor != nullptr)
		{
			LastActor->UnHighlightActors();
		}
		if(CurrentActor != nullptr)
		{
			CurrentActor->HighlightActors();
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if(InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bIsTargetingEnemy = CurrentActor ? true : false;
		bIsAutoRunning = false; //We don't know yet if it is a short press
	}
	
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		UAuraAbilitySystemComponent* AuraAbilitySystemComp = GetAuraAbilitySystemComponent();
		if(IsValid(AuraAbilitySystemComp))
		{
			AuraAbilitySystemComp->AbilityInputTagReleased(InputTag);
		}
		return;
	}
	
	if(bIsTargetingEnemy) // LMB && Hovering over Enemy case
	{
		UAuraAbilitySystemComponent* AuraAbilitySystemComp = GetAuraAbilitySystemComponent();
		if(IsValid(AuraAbilitySystemComp))
		{
			AuraAbilitySystemComp->AbilityInputTagReleased(InputTag);
		}
	}
	else //Click to Move case
	{		
		APawn* ControlledPawn = GetPawn();
		if(IsValid(ControlledPawn) && FollowCursorTime <= ShortPressedThreshold)
		{
			if(UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				SplinePath->ClearSplinePoints();
				for (const FVector& PointLocation : NavigationPath->PathPoints)
				{
					SplinePath->AddSplinePoint(PointLocation, ESplineCoordinateSpace::World);
					DrawDebugSphere(GetWorld(), PointLocation, 8.0f, 8, FColor::Orange, false, 4.0f);
				}
				bIsAutoRunning = true;
			}
			
		}
		FollowCursorTime = 0.0f;
		bIsTargetingEnemy = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if(!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		UAuraAbilitySystemComponent* AuraAbilitySystemComp = GetAuraAbilitySystemComponent();
		if(IsValid(AuraAbilitySystemComp))
		{
			AuraAbilitySystemComp->AbilityInputTagHeld(InputTag);
		}
		return;
	}

	if(bIsTargetingEnemy) // LMB && Hovering over Enemy case
	{
		UAuraAbilitySystemComponent* AuraAbilitySystemComp = GetAuraAbilitySystemComponent();
		if(IsValid(AuraAbilitySystemComp))
		{
			AuraAbilitySystemComp->AbilityInputTagHeld(InputTag);
		}
	}
	else //Click to Move case
	{
		FollowCursorTime += GetWorld()->GetDeltaSeconds();

		FHitResult HitResults;

		if(GetHitResultUnderCursor(ECC_Visibility, false, HitResults))
		{
			CachedDestination = HitResults.ImpactPoint;
		}

		APawn* ControlledPawn = GetPawn();
		if(IsValid(ControlledPawn))
		{
			const FVector DirectionToDestination = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(DirectionToDestination);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent()
{
	if(!IsValid(AuraAbilitySystemComponent))
	{
		UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>());
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AuraAbilitySystemComponent;
}
