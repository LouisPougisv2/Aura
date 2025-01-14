// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"

void UAuraDamageGameplayAbility::CauseDamages(AActor* TargetActor)
{
	if(!IsValid(TargetActor)) return;
	
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffectClass, 1.0f);
	const float DamageMagnitude = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, DamageMagnitude);
		
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefault(AActor* TargetActor) const
{
	FDamageEffectParams DamageEffectParams;
	DamageEffectParams.WorldContext = GetAvatarActorFromActorInfo();
	DamageEffectParams.DamageGameplayEffectClass = DamageGameplayEffectClass;
	DamageEffectParams.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	DamageEffectParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	DamageEffectParams.DamageType = DamageType;
	DamageEffectParams.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	DamageEffectParams.AbilityLevel = GetAbilityLevel();
	DamageEffectParams.DebuffDamage = DebuffDamage;
	DamageEffectParams.DebuffChances = DebuffChances;
	DamageEffectParams.DebuffFrequency = DebuffFrequency;
	DamageEffectParams.DebuffDuration = DebuffDuration;
	DamageEffectParams.DeathImpulseMagnitude = DeathImpulseMagnitude;

	return DamageEffectParams;
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& InTaggedMontages) const
{
	if(InTaggedMontages.Num() > 0)
	{
		const int32 RandomTaggedMontageIndex = FMath::RandRange(0, InTaggedMontages.Num() - 1);
		return InTaggedMontages[RandomTaggedMontageIndex];
	}
	
	return FTaggedMontage();
}

float UAuraDamageGameplayAbility::GetDamageAtLevel(int32 Level) const
{
	return Damage.GetValueAtLevel(Level); 
}
