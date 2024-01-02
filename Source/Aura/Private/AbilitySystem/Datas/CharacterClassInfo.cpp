// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Datas/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetCharacterClassDefaultInfo(ECharacterClass CharacterClass)
{
	return CharacterClassInformationMap.FindChecked(CharacterClass);
}
