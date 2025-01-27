// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraFirebolt.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

FString UAuraFirebolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = GetDamageAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	
	if(Level == 1)
	{
		return FString::Printf(TEXT(
			//Title
			"<Title>FIRE BOLT</>\n\n"

			//Details
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Cost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><CoolDown>%.1f</>\n"

			//Description
			"<Default>Launches a bolt of fire, exploding on impact and dealing: </>"
			"<DamagesFire>%d </><Default>fire damage with a chance to burn</>"
			),

			//Values
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage);
	}
	return FString::Printf(TEXT(
		//Title
		"<Title>FIRE BOLT</>\n\n"

		//Details
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><CoolDown>%.1f</>\n\n"

		//Description
		"<Default>Launches %d bolts of fire, exploding on impact and dealing: </>"
		"<DamagesFire>%d </><Default>fire damage with a chance to burn</>"
		),

		//Values
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, NumProjectiles),
		ScaledDamage);
}

FString UAuraFirebolt::GetNextLevelDescription(int32 NextLevel)
{
	const int32 ScaledDamage = GetDamageAtLevel(NextLevel);
	const float ManaCost = FMath::Abs(GetManaCost(NextLevel));
	const float Cooldown = GetCooldown(NextLevel);
	return FString::Printf(TEXT(
		//Title
		"<Title>NEXT LEVEL</>\n\n"

		//Details
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><CoolDown>%.1f</>\n\n"

		//Description
		"<Default>Launches %d bolts of fire, exploding on impact and dealing: </>"
		"<DamagesFire>%d </><Default>fire damage with a chance to burn</>"),

		//Values
		NextLevel,
		ManaCost,
		Cooldown,
		FMath::Min(NextLevel, NumProjectiles),
		ScaledDamage);
}

void UAuraFirebolt::SpawnProjectiles(const FVector& TargetLocation, AActor* HomingTarget, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{
	if(GetAvatarActorFromActorInfo()->HasAuthority() && ProjectileClass.Get())
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
		FRotator Rotation = (TargetLocation - SocketLocation).Rotation();

		if(bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}

		const FVector ForwardVector = Rotation.Vector();

		const int32 EffectiveNumberProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());
		TArray<FRotator> SpreadOutRotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(ForwardVector, FVector::UpVector, ProjectileSpread, EffectiveNumberProjectiles);

		for (const FRotator& Rot : SpreadOutRotators)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SocketLocation);
			SpawnTransform.SetRotation(Rot.Quaternion());

			AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
				Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			
			Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefault();

			
			if(IsValid(HomingTarget) && HomingTarget->Implements<UCombatInterface>()) //Not clicking on the floor
			{
				Projectile->ProjectileMovementComponent->HomingTargetComponent = HomingTarget->GetRootComponent();
			}
			else
			{
				//Since HomingTargetComponent is a weak pointer, when the projectile dies in HomingTargetComponent or goes out of scope	this new object is not going to be cleaned up
				//Hence why the projectile now has a HomingTargetSceneComponent 
				Projectile->HomingTargetSceneComponent =  NewObject<USceneComponent>(USceneComponent::StaticClass());
				Projectile->HomingTargetSceneComponent->SetWorldLocation(TargetLocation);
				Projectile->ProjectileMovementComponent->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
			}
			Projectile->ProjectileMovementComponent->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
			Projectile->ProjectileMovementComponent->bIsHomingProjectile = bIsHomingProjectile;
			Projectile->FinishSpawning(SpawnTransform);
		}
		
		/*for (auto Rotator : SpreadOutRotators)
		{
			UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SocketLocation + FVector(0.0f, 0.0f, 10.f), SocketLocation + Rotator.Vector() * 75.0f,  1.0f, FLinearColor::Green,60.0f, 1.0f);
		}*/
	}
}
