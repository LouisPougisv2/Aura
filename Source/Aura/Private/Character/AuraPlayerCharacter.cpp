// Copyright Louis Pougis, All Rights Reserved.


#include "Character/AuraPlayerCharacter.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraPlayerCharacter::AAuraPlayerCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CharacterClass = ECharacterClass::Elementalist;
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
}

void AAuraPlayerCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	//TODO Add Attributes Points to PlayerState
	//AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	//check(AuraPlayerState);
	//
	//AuraPlayerState->;
}

void AAuraPlayerCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	//TODO Add Spell Points to PlayerState

}

int32 AAuraPlayerCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	AAuraPlayerState* AuraPlayerState= GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->PlayerLevelUpInfo->FindLevelForExp(InXP);
}

void AAuraPlayerCharacter::LevelUp_Implementation()
{
	//Cosmetic features here
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
