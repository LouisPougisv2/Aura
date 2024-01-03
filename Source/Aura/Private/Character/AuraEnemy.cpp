// Copyright Louis Pougis, All Rights Reserved.


#include "Character/AuraEnemy.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widgets/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));

	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthWidgetComponent->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::HighlightActors()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	WeaponMesh->SetRenderCustomDepth(true);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActors()
{
	GetMesh()->SetRenderCustomDepth(false);
	WeaponMesh->SetRenderCustomDepth(false);
}

int32 AAuraEnemy::GetCharacterLevel() const
{
	return Level;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
	InitAbilityActorInfo();
	//Gives the abilities as long as those abilities are in the Data Asset
	UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent);

	//Setting the Progress bar user widget controller
	UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HealthWidgetComponent->GetUserWidgetObject());
	if(IsValid(AuraUserWidget))
	{
		AuraUserWidget->SetWidgetController(this);
	}

	//Binding the Attribute Value changed delegate + Broadcasting initial values 
	BindAttributeValueChangeDelegates();
}

void AAuraEnemy::InitAbilityActorInfo()
{
	ensureAlways(IsValid(AbilitySystemComponent));
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	if(IsValid(AuraASC))
	{
		AuraASC->OnAbilityInfoSet();
	}
	InitializeDefaultAttributes();
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AAuraEnemy::BindAttributeValueChangeDelegates()
{
	const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	if(AuraAttributeSet)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
		 	[this](const FOnAttributeChangeData& Data)
		 	{
		 	 OnMaxHealthChanged.Broadcast(Data.NewValue);
		 	}
		);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &AAuraEnemy::OnHitReactTagChanged);

		OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	}
}

void AAuraEnemy::OnHitReactTagChanged(const FGameplayTag GameplayTag, int32 NewTagCount)
{
	bShouldHitReact = NewTagCount > 0;
	
	GetCharacterMovement()->MaxWalkSpeed = bShouldHitReact ? 0.0f : BaseWalkSpeed;
	
}
