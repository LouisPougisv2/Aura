// Copyright Louis Pougis, All Rights Reserved.


#include "AuraAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/ExecutionCalculation/ExecCalc_Damage.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	ensureAlways(GEngine);
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//Perfect place to call the InitializeGameplayTags function from our Gameplay Tags Singleton
	FAuraGameplayTags::InitializeNativeGameplayTags();

	//REQUIRED OT US TARGET DATA as it initializes TargetDataScriptStructCache (used by FGameplayAbilityTargetData_SingleTargetHit in UTargetDataUnderMouseAbilityTask)
	//Not required any more since UE5.3
	UAbilitySystemGlobals::Get().InitGlobalData();
}
