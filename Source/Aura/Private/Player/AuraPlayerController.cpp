// Copyright Louis Pougis, All Rights Reserved.


#include "Player/AuraPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/AuraEnemy.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(MappingContext);

	//Binding mapping context to Player Controller
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	ensureAlways(IsValid(LocalPlayerSubsystem));
	LocalPlayerSubsystem->AddMappingContext(MappingContext, 0);

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

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
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
