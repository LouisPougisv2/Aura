// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag)
{
	if(GetAvatarActorFromActorInfo()->HasAuthority() && ProjectileClass.Get())
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		//Rotation.Pitch = 0.0f;	//Making the projectile fly parallel to the ground 

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		const UAbilitySystemComponent* SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

		FGameplayEffectContextHandle GameplayEffectContextHandle = SourceAbilitySystemComponent->MakeEffectContext();
		GameplayEffectContextHandle.SetAbility(this); //Will set AbilityInstanceNotReplicated, AbilityCDO & AbilityLevel for this context handle
		GameplayEffectContextHandle.AddSourceObject(Projectile); // Object this effect was created from
		
		const FGameplayEffectSpecHandle SpecHandle = SourceAbilitySystemComponent->MakeOutgoingSpec(DamageGameplayEffectClass, GetAbilityLevel(), GameplayEffectContextHandle);

		FAuraGameplayTags AuraGameplayTags = FAuraGameplayTags::Get();

		for (auto& DamageTypePair : DamageTypes)
		{
			const float ScaledDamage = DamageTypePair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageTypePair.Key, ScaledDamage);
		}
		
		Projectile->GameplayEffectSpecHandle = SpecHandle;
		Projectile->SetInstigator(Cast<APawn>( GetAvatarActorFromActorInfo()));
		Projectile->FinishSpawning(SpawnTransform);
	}
}
