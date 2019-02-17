// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_GridManager.h"
#include "PaperGroupedSpriteComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TT_BlockManager.h"

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

	SpawnBlockManager();

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

void ATT_GridManager::SpawnBlockManager()
{
	if (BlockManagerClass) {

		ATT_BlockManager* newBlockManager;
		newBlockManager = GetWorld()->SpawnActorDeferred<ATT_BlockManager>(BlockManagerClass, GetActorTransform());
		if (newBlockManager) {
			newBlockManager->SetGridManager(this);
			BlockManager = newBlockManager;

			UGameplayStatics::FinishSpawningActor(newBlockManager, GetActorTransform());
		}
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("BlockManagerClass no set in GridManager. Please set it, and try again."))
	}
}


/*---------- Tile functions ----------*/

void ATT_GridManager::TileHovered(int TileID)
{
	if (!modifiedTiles.Contains(TileID)) {

		TileClearState();
		clickedTile = -1;

		FTransform tempTileTransform;
		instanceGroupedSpriteComp->GetInstanceTransform(TileID, tempTileTransform, true);
		FTransform newTransform = FTransform(tempTileTransform.GetRotation(), tempTileTransform.GetLocation(), FVector(1.1f, 1.1f, 1.1f));

		instanceGroupedSpriteComp->UpdateInstanceTransform(TileID, newTransform, true);

		modifiedTiles.Add(TileID);
	}
}

void ATT_GridManager::TileClicked(int TileID)
{
	if (modifiedTiles.Contains(TileID) && (clickedTile == -1)) {

		TileClearState();

		FTransform tempTileTransform;
		instanceGroupedSpriteComp->GetInstanceTransform(TileID, tempTileTransform, true);
		FTransform newTransform = FTransform(tempTileTransform.GetRotation(), tempTileTransform.GetLocation(), FVector(0.8f, 0.8f, 0.8f));

		instanceGroupedSpriteComp->UpdateInstanceTransform(TileID, newTransform, true);

		int32 clickedTile = TileID;
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
			FTransform newTransform = FTransform(tempTileTransform.GetRotation(), tempTileTransform.GetLocation(), FVector(1.0f, 1.0f, 1.0f));

			instanceGroupedSpriteComp->UpdateInstanceTransform(TileID, newTransform, true);
		}
		modifiedTiles.Empty();
	}
}

FVector ATT_GridManager::GetTileLocation(int TileID)
{
	FTransform tempTransform;
	FVector tempVector;

	if (TileID != -1) {

		instanceGroupedSpriteComp->GetInstanceTransform(TileID, tempTransform, true);
		tempVector = tempTransform.GetLocation();

	}

	return tempVector;
}

