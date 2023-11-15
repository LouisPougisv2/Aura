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

	//Combat Interface
	virtual int32 GetCharacterLevel() const override;
	//EndCombat Interface
	
protected:

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	
	//Not replicated because we'll check the level only on the server for AI controlled enemies
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
	
};
