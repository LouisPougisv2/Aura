// Copyright Louis Pougis, All Rights Reserved.


#include "AuraAssetManager.h"

#include "AuraGameplayTags.h"

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
}
