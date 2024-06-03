// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	
	AAuraPlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual int32 GetCharacterLevel_Implementation() const override;
	
	//Player Interface
	virtual void AddToXp_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	//End Player Interface

protected:

	virtual void InitAbilityActorInfo() override;
};
