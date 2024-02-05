// Copyright Louis Pougis, All Rights Reserved.


#include "AI/AuraAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAuraAIController::AAuraAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	ensureAlwaysMsgf(IsValid(Blackboard), TEXT("BlackboardComponent not created properly in AAuraAIController"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	ensureAlwaysMsgf(IsValid(BehaviorTreeComponent), TEXT("BlackboardComponent not created properly in AAuraAIController"));
}