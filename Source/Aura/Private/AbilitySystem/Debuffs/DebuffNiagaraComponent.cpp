// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Debuffs/DebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if(ASC)
	{
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	}
	else if(CombatInterface)
	{
		//Lambdas holds references of the things they capture in their capture list. If the components get garbage collected (with UPROPERTY),
		//as long as the lambda holds a reference to it, it won't be deleted.
		//BUT a weak lambda has the property of holding references to something without increasing its reference count, so it can be still garbage collected
		CombatInterface->OnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
		{
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
		});
	}

	if(CombatInterface)
	{
		CombatInterface->OnActorDieDelegate().AddUniqueDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag DebufTag, int32 NewCount)
{
	(NewCount > 0) ? Activate() : Deactivate();
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DyingActor)
{
	Deactivate();
}
