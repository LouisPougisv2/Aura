// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouseAbilityTask.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouseAbilityTask* UTargetDataUnderMouseAbilityTask::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouseAbilityTask* TargetDataUnderMouse = NewAbilityTask<UTargetDataUnderMouseAbilityTask>(OwningAbility);
	return TargetDataUnderMouse;
}

void UTargetDataUnderMouseAbilityTask::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if(bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey PredictionKey = GetActivationPredictionKey();
		//As soon as the server is calling Activate(), it'll bind its callback to our delegate
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, PredictionKey).AddUObject(this, &UTargetDataUnderMouseAbilityTask::OnTargetDataReplicatedCallback);

		const bool bHasDelegateAlreadyBeenCalled = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey);
		if(!bHasDelegateAlreadyBeenCalled)
		{
			SetWaitingOnRemotePlayerData();
		}

	}
}

void UTargetDataUnderMouseAbilityTask::SendMouseCursorData()
{
	//Here, we're saying that everything in this function should be predicted. We're asking the server to allow us to do it locally and the server will do it once it'll know about it
	FScopedPredictionWindow ScopedPredictionWindow(AbilitySystemComponent.Get(), AbilitySystemComponent->ScopedPredictionKey);

	const APlayerController* AbilityTaskPlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult HitResult;
	AbilityTaskPlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = HitResult;
	DataHandle.Add(Data);
	auto test = GetActivationPredictionKey();
	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), DataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);

	if(ShouldBroadcastAbilityTaskDelegates())
	{
		MouseDataDelegate.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouseAbilityTask::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	//When the Data has been received by the server, it stored in a specific data structure so we tell the server not to keep it cached anymore
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		MouseDataDelegate.Broadcast(DataHandle);
	}
}
