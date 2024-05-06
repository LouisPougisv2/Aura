// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Datas/AbilityInfo.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoFromTag(const FGameplayTag& AbilityTag, bool bLogIsNotFound) const
{
	for (const FAuraAbilityInfo& AbilityInfo : AbilityInformation)
	{
		if(AbilityInfo.AbilityTag.MatchesTagExact(AbilityTag))
		{
			return AbilityInfo;
		}
	}

	if(bLogIsNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Info for AbilityTag [%s] not found on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}
	
	return FAuraAbilityInfo();
}
