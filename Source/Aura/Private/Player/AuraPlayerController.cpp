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
	AuraInputComponent->BindAction(ShiftInputAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftInputAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);

	AuraInputComponent->BindAbilityAction(AuraInputConfig, this, &AAuraPlayerController::AbilityInputTagPressed, &AAuraPlayerController::AbilityInputTagReleased, &AAuraPlayerController::AbilityInputTagHeld);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

	AutoRunning();
}

void AAuraPlayerController::AutoRunning()
{
	if(!bIsAutoRunning) return;
	
	APawn* ControlledPawn = GetPawn();
	if(IsValid(ControlledPawn))
	{
		//Getting the location on the spline that is the closest to the controlled pawn
		const FVector LocationOnSpline = SplinePath->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector DirectionToClosestLocation = SplinePath->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(DirectionToClosestLocation);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if(DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bIsAutoRunning = false;
		}
	}
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

void AAuraPlayerController::ShiftPressed()
{
	bIsShiftKeyDown = true;
}

void AAuraPlayerController::ShiftReleased()
{
	bIsShiftKeyDown = false;
}

void AAuraPlayerController::CursorTrace()
{
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
		if(IsValid(GetAuraAbilitySystemComponent())) GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);

		return;
	}

	//If we release the LMB while holding Shift key, we still want to let the ASC know that the LMB has been released
	if(IsValid(GetAuraAbilitySystemComponent())) GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);

	if(!bIsTargetingEnemy && !bIsShiftKeyDown) //Click to Move case
	{		
		const APawn* ControlledPawn = GetPawn();
		if(IsValid(ControlledPawn) && FollowCursorTime <= ShortPressedThreshold)
		{
			//Don't forget to turn on Project Settings -> Navigation System -> Allow Client Side Navigation if we want this path to be generated on clients
			if(UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				SplinePath->ClearSplinePoints();
				for (const FVector& PointLocation : NavigationPath->PathPoints)
				{
					SplinePath->AddSplinePoint(PointLocation, ESplineCoordinateSpace::World);
					//DrawDebugSphere(GetWorld(), PointLocation, 8.0f, 8, FColor::Orange, false, 4.0f);
				}

				//Fix issue when clicking at the base of a pilar where the CachedDestination below the cursor wasn't registered as a valid Navigation Point!
				CachedDestination = NavigationPath->PathPoints[NavigationPath->PathPoints.Num() - 1];
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

	if(bIsTargetingEnemy || bIsShiftKeyDown) // LMB && Hovering over Enemy case
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

		if(CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
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

