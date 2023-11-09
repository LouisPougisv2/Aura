// Copyright Louis Pougis, All Rights Reserved.


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
	
}

void AAuraCharacterBase::InitializePrimaryAttributes() const
{
	ensureAlways(IsValid(GetAbilitySystemComponent()));
	ensureAlways(IsValid(DefaultPrimaryAttributes));
	
	const FGameplayEffectContextHandle GameplayEffectContext = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle GameplayEffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes, 1.0f, GameplayEffectContext);

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*GameplayEffectSpecHandle.Data.Get(), GetAbilitySystemComponent());
}
