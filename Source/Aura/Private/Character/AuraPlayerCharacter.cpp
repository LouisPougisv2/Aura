// Copyright Louis Pougis, All Rights Reserved.


#include "Character/AuraPlayerCharacter.h"

#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraPlayerCharacter::AAuraPlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 750.0f;
	CameraBoom->SetRelativeRotation(FRotator(-40.0f, 0.0f, 0.0f));
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCameraComponent"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CharacterClass = ECharacterClass::Elementalist;

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LevelUpNiagaraComponent"));
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false; //We don't want this component to be automatically updated 
}

void AAuraPlayerCharacter::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
	
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	ensureAlways(IsValid(AuraPlayerState));
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);

	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent());
	if(IsValid(AuraASC))
	{
		AuraASC->OnAbilityInfoSet();
	}
	
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	OnAscRegistered.Broadcast(AbilitySystemComponent);
	AttributeSet = AuraPlayerState->GetAttributeSet();

	//Init the Overlay
	if(AAuraPlayerController* PlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if(AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(PlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializeDefaultAttributes();
}

void AAuraPlayerCharacter::Multicast_LevelUpParticle_Implementation() const
{
	ensureAlways(IsValid(LevelUpNiagaraComponent));
	if(!IsValid(LevelUpNiagaraComponent)) return;

	//We want the Niagara effect to face the Camera
	const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
	const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
	LevelUpNiagaraComponent->SetWorldRotation((CameraLocation - NiagaraSystemLocation).Rotation());
	LevelUpNiagaraComponent->Activate(true);
}

void AAuraPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Init ability actor info for the server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AAuraPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Init ability actor info for the client
	InitAbilityActorInfo();
}

int32 AAuraPlayerCharacter::GetCharacterLevel_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)
	return AuraPlayerState->GetCharacterLevel();
}

void AAuraPlayerCharacter::AddToXp_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddToXP(InXP);
}

void AAuraPlayerCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddToLevel(InPlayerLevel);

	//Updating Ability statuses with new level
	UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
	if(IsValid(AuraAbilitySystemComponent))
	{
		AuraAbilitySystemComponent->UpdateAbilityStatuses(AuraPlayerState->GetCharacterLevel());
	}
}

void AAuraPlayerCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	
	AuraPlayerState->AddToAttributePoints(InAttributePoints);
}

void AAuraPlayerCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	
	AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

int32 AAuraPlayerCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	AAuraPlayerState* AuraPlayerState= GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->PlayerLevelUpInfo->FindLevelForExp(InXP);
}

void AAuraPlayerCharacter::LevelUp_Implementation()
{
	Multicast_LevelUpParticle();
}

int32 AAuraPlayerCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerAttributePoints();
}

int32 AAuraPlayerCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerSpellPoints();
}

int32 AAuraPlayerCharacter::GetXP_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerXP();
}

int32 AAuraPlayerCharacter::GetAttributesPointsRewards_Implementation(int32 InLevel) const
{
	AAuraPlayerState* AuraPlayerState= GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->PlayerLevelUpInfo->LevelUpInformations[InLevel].AttributePointsReward;
}

int32 AAuraPlayerCharacter::GetSpellPointsRewards_Implementation(int32 InLevel) const
{
	AAuraPlayerState* AuraPlayerState= GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->PlayerLevelUpInfo->LevelUpInformations[InLevel].SpellPointReward;
}
