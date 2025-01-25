// Copyright Louis Pougis, All Rights Reserved.

#include "AuraAbilityTypes.h"

UScriptStruct* FAuraGameplayEffectContext::GetScriptStruct() const
{
	return StaticStruct();
}

FAuraGameplayEffectContext* FAuraGameplayEffectContext::Duplicate() const
{
	FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
	*NewContext = *this;
	if (GetHitResult())
	{
		// Does a deep copy of the hit result
		NewContext->AddHitResult(*GetHitResult(), true);
	}
	return NewContext;
}

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint32 RepBits = 0;
	if(Ar.IsSaving())
	{
		if(bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid() )
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		if(bIsBlockedHit)
		{
			RepBits |= 1 << 7;
		}
		if(bIsCriticalHit)
		{
			RepBits |= 1<< 8;
		}
		if(bIsDebuffSuccessful)
		{
			RepBits |= 1 << 9;
		}
		if(DebuffDamage > 0.0f)
		{
			RepBits |= 1 << 10;
		}
		if(DebuffDuration > 0.0f)
		{
			RepBits |= 1 << 11;
		}
		if(DebuffFrequency > 0.0f)
		{
			RepBits |= 1 << 12;
		}
		if(DamageType.IsValid())
		{
			RepBits |= 1 << 13;
		}
		if(!DeathImpulse.IsZero())
		{
			RepBits = 1 << 14;
		}
		if(KnockbackMagnitude > 0.0f)
		{
			RepBits |= 1 << 15;
		}
		if(!KnockbackForce.IsZero())
		{
			RepBits = 1 << 16;
		}
		if(KnockbackChance > 0.0f)
		{
			RepBits |= 1 << 17;
		}
	}

	Ar.SerializeBits(&RepBits, 17);

	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}

	if(RepBits & (1 << 7))
	{
		Ar << bIsBlockedHit;
	}

	if(RepBits & (1 << 8))
	{
		Ar << bIsCriticalHit;
	}

	if(RepBits & (1 << 9))
	{
		Ar << bIsDebuffSuccessful;
	}

	if(RepBits & (1 << 10))
	{
		Ar << DebuffDamage;
	}
	if(RepBits & (1 << 11))
	{
		Ar << DebuffDuration;
	}
	if(RepBits & (1 << 12))
	{
		Ar << DebuffFrequency;
	}
	if(RepBits & (1 << 13))
	{
		//If this condition is true, we're loading so the DamageType has already been serialized
		if(Ar.IsLoading())
		{
			if(!DamageType.IsValid())
			{
				DamageType = TSharedPtr<FGameplayTag>(new FGameplayTag());
			}
		}	
		DamageType->NetSerialize(Ar, Map, bOutSuccess);
	}
	if(RepBits & (1 << 14))
	{
		DeathImpulse.NetSerialize(Ar, Map, bOutSuccess);
	}
	if(RepBits & (1 << 15))
	{
		Ar << KnockbackMagnitude;
	}
	if(RepBits & (1 << 16))
	{
		KnockbackForce.NetSerialize(Ar, Map, bOutSuccess);
	}
	if(RepBits & (1 << 17))
	{
		Ar << KnockbackChance;
	}

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}
	
	bOutSuccess = true;
	return true;
}
