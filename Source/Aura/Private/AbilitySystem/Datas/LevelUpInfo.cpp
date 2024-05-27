// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Datas/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForExp(int32 Exp) const
{
	int32 CurrentPlayerLevel = 1;
	bool bIsSearching = true;

	while (bIsSearching)
	{
		//Level 1 Information[0] is just a placeholder (it is meaningless)
		//LevelUpInformations[1] = Level 1 Information
		//LevelUpInformations[2] = Level 2 Information
		if(LevelUpInformations.Num() - 1 <= CurrentPlayerLevel) return CurrentPlayerLevel;

		if(Exp >= LevelUpInformations[CurrentPlayerLevel].LevelUpRequirement)
		{
			++CurrentPlayerLevel;
		}
		else
		{
			bIsSearching = false;
		}
	}

	return CurrentPlayerLevel;
}
