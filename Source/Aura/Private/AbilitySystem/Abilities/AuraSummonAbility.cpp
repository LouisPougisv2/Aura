// Copyright Louis Pougis, All Rights Reserved.


#include "AbilitySystem/Abilities/AuraSummonAbility.h"
//#include "Kismet/KismetSystemLibrary.h"

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
		FVector ChosenLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);

		//Line Trace to ensure the ChosenPoint will always be on the floor
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, ChosenLocation + FVector(0.0f, 0.0f, 400.0f), ChosenLocation - FVector(0.0f, 0.0f, 400.0f), ECC_Visibility);
		if(HitResult.bBlockingHit)
		{
			ChosenLocation = HitResult.ImpactPoint;
		}
		//End of Line Trace
		
		SpawnLocations.Add(ChosenLocation);
		//UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.0f, FLinearColor::Red, 4.0f);\
		//DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 10.0f, 12.0f, FColor::Orange, false, 5.0f);
		//DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 10.0f, 12.0f, FColor::Purple,false, 5.0f);
		//DrawDebugSphere(GetWorld(), ChosenLocation, 10.0f, 12.0f, FColor::Green,false, 2.0f);
	}
	
	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	if(MinionClasses.Num() <= 0) return nullptr;
	return MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)];
}
