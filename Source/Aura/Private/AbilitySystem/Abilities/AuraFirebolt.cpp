// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraFirebolt.h"

#include "AuraGameplayTags.h"

FString UAuraFirebolt::GetDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	
	if(Level == 1)
	{
		return FString::Printf(TEXT(
			//Title
			"<Title>FIRE BOLT</>\n\n"

			//Details
			"<Small>Level: </><Level>%d</>\n"
			"<Small>Cost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><CoolDown>%.1f</>\n"

			//Description
			"<Default>Launches a bolt of fire, exploding on impact and dealing: </>"
			"<DamagesFire>%d </><Default>fire damage with a chance to burn</>"
			),

			//Values
			Level,
			ManaCost,
			Cooldown,
			Damage);
	}
	return FString::Printf(TEXT(
		//Title
		"<Title>FIRE BOLT</>\n\n"

		//Details
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><CoolDown>%.1f</>\n\n"

		//Description
		"<Default>Launches %d bolts of fire, exploding on impact and dealing: </>"
		"<DamagesFire>%d </><Default>fire damage with a chance to burn</>"
		),

		//Values
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, NumProjectiles),
		Damage);
}

FString UAuraFirebolt::GetNextLevelDescription(int32 NextLevel)
{
	const int32 Damage = GetDamageByDamageType(NextLevel, FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost = FMath::Abs(GetManaCost(NextLevel));
	const float Cooldown = GetCooldown(NextLevel);
	return FString::Printf(TEXT(
		//Title
		"<Title>NEXT LEVEL</>\n\n"

		//Details
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><CoolDown>%.1f</>\n\n"

		//Description
		"<Default>Launches %d bolts of fire, exploding on impact and dealing: </>"
		"<DamagesFire>%d </><Default>fire damage with a chance to burn</>"),

		//Values
		NextLevel,
		ManaCost,
		Cooldown,
		FMath::Min(NextLevel, NumProjectiles),
		Damage);
}
