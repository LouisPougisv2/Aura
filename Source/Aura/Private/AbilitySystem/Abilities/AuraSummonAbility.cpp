// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector ForwardVector = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	const float DeltaSpread = SpawnConeSpread / NumMinionsToSpawn;
	//const FVector RightBoundaryOfSpread = ForwardVector.RotateAngleAxis(SpawnConeSpread / 2.0f, FVector::UpVector);

	const FVector LeftBoundaryOfSpread = ForwardVector.RotateAngleAxis(-SpawnConeSpread / 2.0f, FVector::UpVector);
	TArray<FVector> SpawnLocations;
	for(int32 i = 0; i < NumMinionsToSpawn; ++i)
	{
		const FVector Direction = LeftBoundaryOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		const FVector ChosenLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);
		SpawnLocations.Add(ChosenLocation);
		
		//UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.0f, FLinearColor::Red, 4.0f);\
		//DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 10.0f, 12.0f, FColor::Orange, false, 5.0f);
		//DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 10.0f, 12.0f, FColor::Purple,false, 5.0f);
		//DrawDebugSphere(GetWorld(), ChosenLocation, 10.0f, 12.0f, FColor::Green,false, 5.0f);
	}
	
	return SpawnLocations;
}
