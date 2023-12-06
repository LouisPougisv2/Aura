// Copyright Louis Pougis, All Rights Reserved.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraPlayerCharacter.h"
#include "Inputs/AuraEnhancedInputComponent.h"
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
	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, FString::Printf(TEXT("Key [%s] Pressed! \n"), *InputTag.ToString()));
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	UAuraAbilitySystemComponent* AuraAbilitySystemComp = GetAuraAbilitySystemComponent();
	if(IsValid(AuraAbilitySystemComp))
	{
		AuraAbilitySystemComp->AbilityInputTagReleased(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	UAuraAbilitySystemComponent* AuraAbilitySystemComp = GetAuraAbilitySystemComponent();
	if(IsValid(AuraAbilitySystemComp))
	{
		AuraAbilitySystemComp->AbilityInputTagHeld(InputTag);
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
