// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraBeamSpellGameplayAbility.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

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

void UAuraBeamSpellGameplayAbility::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	ensureAlways(IsValid(OwnerCharacter));
	FVector SocketLocation = FVector();
	if(OwnerCharacter->Implements<UCombatInterface>())
	{
		if(USkeletalMeshComponent* WeaponMesh = ICombatInterface::Execute_GetWeaponMesh(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.AddUnique(OwnerCharacter);
			SocketLocation = WeaponMesh->GetSocketLocation(FName("TipSocket"));
			FHitResult HitResult;
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter,
				SocketLocation,
				BeamTargetLocation,
				10.0f,
				ETraceTypeQuery::TraceTypeQuery1,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				HitResult,
				true);

			if(HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
		
	}
}
