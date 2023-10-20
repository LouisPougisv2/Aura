// Copyright Louis Pougis, All Rights Reserved.


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	//Application policies
	if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	
	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if(InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	//Application policies
	if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if(InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	//Removal Policies
	if(InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if(!IsValid(TargetActorASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> EffectHandlePair : ActiveEffectHandlesMap)
		{
			if(EffectHandlePair.Value == TargetActorASC)
			{
				//Removing the Gameplay Effect (leaving -1 as 2nd param results that the whole stack being cleared)
				TargetActorASC->RemoveActiveGameplayEffect(EffectHandlePair.Key, 1);

				//Mark that handle to be removed from our map
				HandlesToRemove.Add(EffectHandlePair.Key);
			}
		}
	
		for (FActiveGameplayEffectHandle& ActiveGameplayEffectHandle : HandlesToRemove)
		{
			ActiveEffectHandlesMap.FindAndRemoveChecked(ActiveGameplayEffectHandle);
		}
		
	}
}


void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	//To following line replace the usual :
	// IAbilitySystemInterface* ASCInterface = CastCheck<IAbilitySystemInterface>(TargetActor);
	// if(ASCInterface) UAbilitySystemComponent* AbilitySystemComponent = ASCInterface->GetAbilitySystemComponent();
	UAbilitySystemComponent* TargetActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(!IsValid(TargetActorASC)) return;
	ensureAlways(IsValid(GameplayEffectClass));

	FGameplayEffectContextHandle EffectContextHandle = TargetActorASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetActorASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectContextHandle);
	
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetActorASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	const bool bIsDurationPolicyInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;

	//We store the effect only if we plan on removing it in the future (so it only matches the Infinite policy case)
	if(bIsDurationPolicyInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		//Mapping ActiveGameplayEffect handle to ASC
		ActiveEffectHandlesMap.Add(ActiveGameplayEffectHandle, TargetActorASC);
	}
}


