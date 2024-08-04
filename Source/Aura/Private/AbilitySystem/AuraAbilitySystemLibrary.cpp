// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"


bool UAuraAbilitySystemLibrary::MakeWidgetControllerParam(const UObject* WorldContextObject, FWidgetControllerParam& OutWidgetControllerParam, AAuraHUD*& OutAuraHUD)
{
	if(APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutAuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD());
		if(OutAuraHUD)
		{
			AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
			UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();

			OutWidgetControllerParam.PlayerController = PlayerController;
			OutWidgetControllerParam.PlayerState = PlayerState;
			OutWidgetControllerParam.AbilitySystemComponent = AbilitySystemComponent;
			OutWidgetControllerParam.AttributeSet = AttributeSet;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParam WidgetControllerParam;
	AAuraHUD* OutAuraHUD = nullptr;
	
	if(MakeWidgetControllerParam(WorldContextObject, WidgetControllerParam, OutAuraHUD))
	{
		return OutAuraHUD->GetOverlayWidgetController(WidgetControllerParam);
	}
	return nullptr;
}

UAttributesMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributesMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParam WidgetControllerParam;
	AAuraHUD* OutAuraHUD = nullptr;
	
	if(MakeWidgetControllerParam(WorldContextObject, WidgetControllerParam, OutAuraHUD))
	{
		return OutAuraHUD->GetAttributesMenuWidgetController(WidgetControllerParam);
	}
	return nullptr;
}

USpellMenuAuraWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParam WidgetControllerParam;
	AAuraHUD* OutAuraHUD = nullptr;
	
	if(MakeWidgetControllerParam(WorldContextObject, WidgetControllerParam, OutAuraHUD))
	{
		return OutAuraHUD->GetSpellMenuWidgetController(WidgetControllerParam);
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 Level, UAbilitySystemComponent* AbilitySystemComponent)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	const FCharacterClassDefaultInfo CharacterDefaultClassInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	
	//ApplyGameplay Primary Attributes Effect Spec 
		//Setting the source object of the Gameplay Effect to avoid future crash
	FGameplayEffectContextHandle PrimaryAttributesContextHandle = AbilitySystemComponent->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AbilitySystemComponent->GetAvatarActor());

	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CharacterDefaultClassInfo.PrimaryAttributesGameplayEffect, Level, PrimaryAttributesContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get(), AbilitySystemComponent->ScopedPredictionKey);

	//ApplyGameplay Secondary Attributes Effect Spec
	FGameplayEffectContextHandle SecondaryAttributesContextHandle = AbilitySystemComponent->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AbilitySystemComponent->GetAvatarActor());
	
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributesGameplayEffect, Level, SecondaryAttributesContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get(), AbilitySystemComponent->ScopedPredictionKey);

	//ApplyGameplay Vital Attributes Effect Spec
	FGameplayEffectContextHandle VitalAttributesContextHandle = AbilitySystemComponent->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AbilitySystemComponent->GetAvatarActor());
	
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CharacterClassInfo->VitalAttributesGameplayEffect, Level, VitalAttributesContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get(), AbilitySystemComponent->ScopedPredictionKey);
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* AbilitySystemComponent, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if(!IsValid(CharacterClassInfo)) return;
	
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}

	//Giving all start up ability to Enemy, based on his CharacterClass
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	if(AbilitySystemComponent->GetAvatarActor()->Implements<UCombatInterface>())
	{
		for (TSubclassOf<UGameplayAbility> CommonDefaultAbility : DefaultInfo.CommonDefaultAbilities)
        {
        	FGameplayAbilitySpec CommonDefaultAbilitySpec = FGameplayAbilitySpec(CommonDefaultAbility, ICombatInterface::Execute_GetCharacterLevel(AbilitySystemComponent->GetAvatarActor()));
        	AbilitySystemComponent->GiveAbility(CommonDefaultAbilitySpec);
        }
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(!IsValid(AuraGameMode)) return nullptr;

	return AuraGameMode->CharacterClassInfo.Get();
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(!IsValid(AuraGameMode)) return nullptr;

	return AuraGameMode->AbilityInfo.Get();
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->IsHitBlocked();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGameplayEffectContext->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsBlocked)
{
	if(FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetIsBlockedHit(bIsBlocked);
	}
}

void UAuraAbilitySystemLibrary::SetCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsCritical)
{
	if(FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetIsCriticalHit(bIsCritical);
	}
}

void UAuraAbilitySystemLibrary::GetAlivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingPlayers, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> OutOverlaps;
	
	if(UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(OutOverlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : OutOverlaps)
		{
			// ImplementsCombatInterface && IsAlive
			if(Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingPlayers.AddUnique(Overlap.GetActor());
			}
		}
	}
}

bool UAuraAbilitySystemLibrary::AreFriends(AActor* FirstActor, AActor* SecondActor)
{
	if(!FirstActor->Implements<UCombatInterface>() || !SecondActor->Implements<UCombatInterface>()) return false;
	
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	return bBothArePlayers || bBothAreEnemies;
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if(!IsValid(CharacterClassInfo)) return 0;

	const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);
	return static_cast<int32>(XPReward);
}
