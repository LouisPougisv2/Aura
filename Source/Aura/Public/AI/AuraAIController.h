// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AuraAIController.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	AAuraAIController();

protected:

	UPROPERTY()
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComponent;
};
