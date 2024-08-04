// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamages(AActor* TargetActor)
{
	if(!IsValid(TargetActor)) return;
	
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffectClass, 1.0f);
	for (TTuple<FGameplayTag, FScalableFloat> DamageTypePair : DamageTypes)
	{
		const float DamageMagnitude = DamageTypePair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageTypePair.Key, DamageMagnitude);
	}
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
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

float UAuraDamageGameplayAbility::GetDamageByDamageType(int32 Level, const FGameplayTag& DamageType) const
{
	checkf(DamageTypes.Contains(DamageType), TEXT("Gameplay Ability %s doesn't contain Damage type %s"), *GetNameSafe(this), *DamageType.ToString());
	return DamageTypes[DamageType].GetValueAtLevel(Level);
}
