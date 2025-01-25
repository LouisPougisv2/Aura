// Copyright Louis Pougis, All Rights Reserved.


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere);
	CollisionSphere->SetCollisionObjectType(ECC_Projectile);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 550.0f;
	ProjectileMovementComponent->MaxSpeed = 550.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;	//We don't want any gravity one our projectiles
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
	
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

	if(LoopingSound)
	{
		LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	}
	
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Other == GetInstigator()) return;

	if(DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor() == Other) return;

	//Preventing enemy to damage each other (preventing players to hit each other too)
	if(UAuraAbilitySystemLibrary::AreFriends(DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor(), Other)) return;

	ImpactSoundAndEffect();	
	if(HasAuthority())
	{
		if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;

			const bool bShouldKnockBack = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance;
			if(bShouldKnockBack)
			{
				//const FVector KnockBackDirection = GetActorForwardVector().RotateAngleAxis(45.0f, GetActorRightVector());
				FRotator Rotation = GetActorRotation();
				Rotation.Pitch = 45.0f;
				const FVector KnockBackDirection = Rotation.Vector();
				const FVector KnockBackForce = KnockBackDirection * DamageEffectParams.KnockbackMagnitude;
				DamageEffectParams.KnockbackForce = KnockBackForce;
			}
			
			UAuraAbilitySystemLibrary::ApplyDamageEffectToTarget(DamageEffectParams);
		}
		Destroy();
	}
	else
	{
		bHit = true;
	}
}

//If destroyed is called before OnSphereOverlap (only pertains to clients)
void AAuraProjectile::Destroyed()
{
	if(!bHit && !HasAuthority())
	{
		ImpactSoundAndEffect();
	}
	Super::Destroyed();
}

void AAuraProjectile::ImpactSoundAndEffect()
{
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
	}

	if(ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	}
	if(LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}
	bHit = true;
}

