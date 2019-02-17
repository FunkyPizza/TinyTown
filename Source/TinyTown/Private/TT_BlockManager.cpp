// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_BlockManager.h"
#include "TT_GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "PaperGroupedSpriteComponent.h"


/*---------- Primary functions ----------*/

ATT_BlockManager::ATT_BlockManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATT_BlockManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATT_BlockManager::SetGridManager(ATT_GridManager* newGridManager)
{
	GridManager = newGridManager;
}


/*---------- Block building functions ----------*/

void ATT_BlockManager::CreateBuildingOnTile(int TileID)
{
	if (!GridManager) {
		UE_LOG(LogTemp, Log, TEXT("Grid Maker not set in BlockManager, cannot build"));
		return;
	}

	FTransform tileTransform = FTransform(FRotator(0, 0, 0), GridManager->GetTileLocation(TileID), FVector(1, 1, 1));
	SpawnedActor = GetWorld()->SpawnActor<AActor>(BlockToSpawn, tileTransform);
}


