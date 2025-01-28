// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraBeamSpellGameplayAbility.h"
#include "GameFramework/Character.h"

void UAuraBeamSpellGameplayAbility::StoreMouseDataInfo(const FHitResult& MouseHitResult)
{
	if(MouseHitResult.bBlockingHit)
	{
		MouseHitLocation = MouseHitResult.ImpactPoint;
		MouseHitActor = MouseHitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpellGameplayAbility::StoreOwnerVariables()
{
	if(CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();

		//We're storing the OwnerCharacter to be able to get its movement component to disable movement while casting this spell
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}
