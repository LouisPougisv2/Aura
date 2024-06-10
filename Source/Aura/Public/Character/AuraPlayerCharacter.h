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
	virtual int32 GetXP_Implementation()const  override;
	virtual int32 GetAttributesPointsRewards_Implementation(int32 InLevel) const override;
	virtual int32 GetSpellPointsRewards_Implementation(int32 InLevel) const override;
	virtual void AddToXp_Implementation(int32 InXP) override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual void LevelUp_Implementation() override;
	//End Player Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCameraComponent> TopDownCameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_LevelUpParticle() const;
};
