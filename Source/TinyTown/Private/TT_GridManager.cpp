// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_GridManager.h"
#include "PaperGroupedSpriteComponent.h"

ATT_GridManager::ATT_GridManager()
{
	/*---------- Components creation ----------*/
	instanceGroupedSpriteComp = CreateDefaultSubobject<UPaperGroupedSpriteComponent>(TEXT("GroupedSpriteComp"));
	instanceGroupedSpriteComp->SetWorldTransform(FTransform(GetActorLocation(), FVector(-90, 0, 0), GetActorScale()));
	instanceGroupedSpriteComp->SetupAttachment(RootComponent);


	/*---------- Constructor ----------*/

	CalculateTileLocations(sizeX, sizeY, GetActorLocation(), distanceBetweenTiles);
	SpawnTileInstances();
	
}

void ATT_GridManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATT_GridManager::CalculateTileLocations(int x, int y, FVector& Center, float distance)
{
	for (int i = 0; i < (y - 1); i++) {
		for (int j = 0; j < (x - 1); j++) {

			//Calculating new location (will spawn around center vector hence the very long expression)
			FVector newLocation = FVector(Center + (FVector(distance * 0.5f) + (distance * (FVector((j - (float(x) / 2)), (i - (float(y) / 2)), 0.0f)))));

			tileLocations.Add(newLocation);
		}
	}
}

void ATT_GridManager::SpawnTileInstances()
{
	for (int i = 0; i < tileLocations.Num(); i++) {

		FTransform tileTransform = FTransform(FTransform(tileLocations[i], instanceGroupedSpriteComp->GetComponentRotation(), GetActorScale()));
		instanceGroupedSpriteComp->AddInstance(tileTransform, tileSpriteNormal, true, FLinearColor::White);
	}
}

