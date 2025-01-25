// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:

	UDebuffNiagaraComponent();
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;

protected:

	virtual void BeginPlay() override;
	void DebuffTagChanged(const FGameplayTag DebufTag, int32 NewCount);

	UFUNCTION()
	void OnOwnerDeath(AActor* DyingActor);
};
