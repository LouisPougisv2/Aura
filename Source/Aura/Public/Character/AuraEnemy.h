// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

#define CUSTOM_DEPTH_RED 250

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:

	AAuraEnemy();

	//Enemy interface
	virtual void HighlightActors() override;
	virtual void UnHighlightActors() override;
	//End enemy interface
	
protected:

	virtual void BeginPlay() override;
	
};
