// Copyright Louis Pougis, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Datas/CharacterClassInfo.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
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
	virtual void Die() override;
	//EndCombat Interface

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bShouldHitReact = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.0f;
	
protected:

	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	void OnHitReactTagChanged(const FGameplayTag GameplayTag, int32 NewTagCount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	
	//Not replicated because we'll check the level only on the server for AI controlled enemies
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UWidgetComponent> HealthWidgetComponent;

private:

	void BindAttributeValueChangeDelegates();
	
};
