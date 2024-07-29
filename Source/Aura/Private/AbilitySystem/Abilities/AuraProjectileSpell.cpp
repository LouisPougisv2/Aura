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

FString UAuraProjectileSpell::GetDescription(int32 Level)
{
	const int32 Damage = DamageTypes[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);
	
	if(Level == 1)
	{
		return FString::Printf(TEXT("<Title>FIRE BOLT</>\n<Default>Launches a bolt of fire, exploding on impact and dealing: </><DamagesFire>%d </><Default>fire damage with a chance to burn</>\n\n<Small>Level: %d</>"), Damage, Level);
	}
	return FString::Printf(TEXT("<Title>FIRE BOLT</>\n<Default>Launches %d bolts of fire, exploding on impact and dealing: </><DamagesFire>%d </><Default>fire damage with a chance to burn</>\n\n<Small>Level: %d</>"), FMath::Min(Level, NumProjectiles), Damage, Level);
}

FString UAuraProjectileSpell::GetNextLevelDescription(int32 NextLevel)
{
	const int32 Damage = DamageTypes[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(NextLevel);
	return FString::Printf(TEXT("<Title>NEXT LEVEL</>\n<Default>Launches %d bolts of fire, exploding on impact and dealing: </><DamagesFire>%d </><Default>fire damage with a chance to burn</>\n\n<Small>Level: %d</>"), FMath::Min(NextLevel, NumProjectiles), Damage, NextLevel);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{
	if(GetAvatarActorFromActorInfo()->HasAuthority() && ProjectileClass.Get())
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		//Rotation.Pitch = 0.0f;	//Making the projectile fly parallel to the ground 
		
		if(bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}
		
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
