// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouseAbilityTask.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseDataSignature, const FGameplayAbilityTargetDataHandle& , DataHandle);
/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouseAbilityTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	
	//This will become an output execution pin on the node
	UPROPERTY(BlueprintAssignable)
	FMouseDataSignature MouseDataDelegate;
	
	UFUNCTION(BlueprintCallable, Category = "Ability | Task", meta = (DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"), BlueprintInternalUseOnly)
	static UTargetDataUnderMouseAbilityTask* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

private:

	virtual void Activate() override;
	void SendMouseCursorData();

	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
};
