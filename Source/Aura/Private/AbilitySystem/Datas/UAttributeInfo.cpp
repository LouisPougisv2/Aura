// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Datas/UAttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& GameplayTag, bool bLogNotfound) const
{
	for (const FAuraAttributeInfo& AttributeInfo : AttributesInformation)
	{
		if(AttributeInfo.AttributeTag.MatchesTagExact(GameplayTag))
		{
			return AttributeInfo;
		}
	}
	if(bLogNotfound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute with Tag [%s] not found on AttributesInfo [%s]"), *GameplayTag.ToString(), *GetNameSafe(this));
	}
	return FAuraAttributeInfo();
}
