// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_GridManager.h"
#include "PaperGroupedSpriteComponent.h"

/*---------- Primary functions ----------*/

ATT_GridManager::ATT_GridManager()
{
	/*---------- Components creation ----------*/
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	instanceGroupedSpriteComp = CreateDefaultSubobject<UPaperGroupedSpriteComponent>(TEXT("GroupedSpriteComp"));
	instanceGroupedSpriteComp->SetRelativeLocationAndRotation(GetActorLocation(), FRotator(0, 0, -90));
	instanceGroupedSpriteComp->SetupAttachment(Root);

	/*---------- Setting defaults ----------*/
	PrimaryActorTick.bCanEverTick = false;

}

void ATT_GridManager::OnConstruction(const FTransform& Transform)
{
	if ( sizeX * sizeY != tileLocations.Num()) {

		instanceGroupedSpriteComp->ClearInstances();
		tileLocations.Empty();

		SpawnTiles(sizeX, sizeY, GetActorLocation(), distanceBetweenTiles);
	}
	Super::OnConstruction(Transform);
}

void ATT_GridManager::BeginPlay()
{
	Super::BeginPlay();

}


/*---------- Tile Spawning ----------*/

void ATT_GridManager::SpawnTiles(int x, int y, FVector Center, float distance)
{
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {

			//Calculating new location (will spawn around center vector hence the very long expression). Taken from "2D Grid Execution Macro".
			FVector newLocation = FVector(Center + (FVector(distance * 0.5f, distance * 0.5f, 0.0f) + (distance * (FVector((j - (float(x) / 2)), (i - (float(y) / 2)), 0.0f)))));

			tileLocations.Add(newLocation);

			FTransform tileTransform = FTransform(FRotator(0, 0, -90), newLocation, FVector(1, 1, 1));

			instanceGroupedSpriteComp->AddInstance(tileTransform, tileSpriteNormal, true, FLinearColor::White);
		}
	}

}


/*---------- Tile functions ----------*/

void ATT_GridManager::TileHovered(int TileID)
{
	if (!modifiedTiles.Contains(TileID) || clickedTile ==0 ) {

		TileClearState();
		clickedTile = 0;

		FTransform tempTileTransform;
		instanceGroupedSpriteComp->GetInstanceTransform(TileID, tempTileTransform, true);
		instanceGroupedSpriteComp->RemoveInstance(TileID);

		FTransform newTransform = FTransform(tempTileTransform.GetRotation(), tempTileTransform.GetLocation(), FVector(1.1f, 1.1f, 1.1f));
		modifiedTiles.Add(instanceGroupedSpriteComp->AddInstance(newTransform, tileSpriteNormal, true, FLinearColor::White));
	}
}

void ATT_GridManager::TileClicked(int TileID)
{
	if (clickedTile == 0) {

		TileClearState();

		FTransform tempTileTransform;
		instanceGroupedSpriteComp->GetInstanceTransform(TileID, tempTileTransform, true);
		instanceGroupedSpriteComp->RemoveInstance(TileID);

		FTransform newTransform = FTransform(tempTileTransform.GetRotation(), tempTileTransform.GetLocation(), FVector(0.8f, 0.8f, 0.8f));

		int32 clickedTile = instanceGroupedSpriteComp->AddInstance(newTransform, tileSpriteNormal, true, FLinearColor::White);
		modifiedTiles.Add(clickedTile);
	}
}

void ATT_GridManager::TileClearState()
{
	if (modifiedTiles.Num() > 0) {

		for (int i = 0; i < modifiedTiles.Num(); ++i) {

			int TileID = modifiedTiles[i];

			FTransform tempTileTransform;
			instanceGroupedSpriteComp->GetInstanceTransform(TileID, tempTileTransform, true);
			instanceGroupedSpriteComp->RemoveInstance(TileID);

			FTransform newTransform = FTransform(tempTileTransform.GetRotation(), tempTileTransform.GetLocation(), FVector(1.0f, 1.0f, 1.0f));
			instanceGroupedSpriteComp->AddInstance(newTransform, tileSpriteNormal, true, FLinearColor::White);
		}
		modifiedTiles.Empty();
	}
}

