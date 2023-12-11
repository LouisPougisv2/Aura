// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(HasAuthority(&ActivationInfo) && ProjectileClass.Get())
	{
		ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
		if(CombatInterface)
		{
			const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SocketLocation);
			//TODO : Set projectile Rotation

			AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
				Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			//TODO : Give the Projectile a Gameplay Effect for causing Damages
			
			Projectile->FinishSpawning(SpawnTransform);
		}
	}
}
