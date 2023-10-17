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
	FGameplayEffectSpecHandle EffectSpecHandle = TargetActorASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectContextHandle);
	TargetActorASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}


