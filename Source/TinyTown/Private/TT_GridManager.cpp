// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_GridManager.h"
#include "PaperGroupedSpriteComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TT_BlockManager.h"
#include "TimerManager.h"
#include "Engine/TextRenderActor.h"
#include "Components/TextRenderComponent.h"

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
	Super::OnConstruction(Transform);

		// Refresh the grid only when the grid size has been changed. Enables moving the grid with refreshing all instances.
		instanceGroupedSpriteComp->ClearInstances();
		tileLocations.Empty();

		for (ATextRenderActor* i : tileIDActors)
		{
			i->Destroy(true, true);
		}
		tileIDActors.Empty();

		SpawnTiles(gridSizeX, gridSizeY, GetActorLocation(), distanceBetweenTiles);

}

void ATT_GridManager::BeginPlay()
{
	Super::BeginPlay();

	SpawnBlockManager();

	tileIDs.Empty();

	for (int i = 0; i < instanceGroupedSpriteComp->GetInstanceCount(); i++)
	{
		tileIDs.Add(i);
	}
}


/*---------- Accessor functions ----------*/

FVector ATT_GridManager::GetTileLocation(int tileID)
{
	FTransform tempTransform;
	FVector tempVector;

	if (tileID != -1)
	{
		instanceGroupedSpriteComp->GetInstanceTransform(tileID, tempTransform, true);
		tempVector = tempTransform.GetLocation();
	}

	return tempVector;
}

TArray<int> ATT_GridManager::GetTileNeighbours(int tileID, bool allowDiagonalPaths, TArray<int>& AllNeighboursTileID)
{
	TArray<int> neighbours;
	TArray<int> neighboursAll;
	FVector2D gridSize = GetGridSize();
	int currentNeighbour;
	int tempRow;


	// Right tile
	currentNeighbour = tileID + gridSize.X;
	if (IsTileValid(currentNeighbour))
	{
		neighboursAll.Add(currentNeighbour);
		neighbours.Add(currentNeighbour);
	}
	else
	{
		neighboursAll.Add(-1);
	}

	// Bottom right
	currentNeighbour = tileID + gridSize.X - 1;
	tempRow = FMath::TruncToInt(float(tileID / gridSize.X));
	if (allowDiagonalPaths && tileID % int(gridSize.X) != 0 && tileID != tempRow * gridSize.X && IsTileValid(currentNeighbour))
	{
		neighboursAll.Add(currentNeighbour);
		neighbours.Add(currentNeighbour);
	}
	else if (allowDiagonalPaths)
	{
		neighboursAll.Add(-1);
	}

	// Bottom tile
	currentNeighbour = tileID - 1;
	tempRow = FMath::TruncToInt(float(tileID / gridSize.X));
	if (tileID % int(gridSize.X) != 0 && IsTileValid(currentNeighbour) && tileID != tempRow * gridSize.X) 
	{
		neighboursAll.Add(currentNeighbour);
		neighbours.Add(currentNeighbour);
	}
	else
	{
		neighboursAll.Add(-1);
	}

	// Bottom left
	currentNeighbour = tileID - gridSize.X - 1;
	tempRow = FMath::TruncToInt(float(tileID / gridSize.X));
	if (allowDiagonalPaths && tileID % int(gridSize.X) != 0 && tileID != tempRow * gridSize.X && IsTileValid(currentNeighbour))
	{
		neighboursAll.Add(currentNeighbour);
		neighbours.Add(currentNeighbour);		
	}
	else if(allowDiagonalPaths)
	{
		neighboursAll.Add(-1);
	}

	// Left tile
	currentNeighbour = tileID - gridSize.X;
	if (IsTileValid(currentNeighbour))
	{
		neighboursAll.Add(currentNeighbour);
		neighbours.Add(currentNeighbour);
	}
	else
	{
		neighboursAll.Add(-1);
	}

	// Top left
	currentNeighbour = tileID - gridSize.X + 1;
	tempRow = FMath::TruncToInt(float(tileID + 1 / gridSize.X));
	if (allowDiagonalPaths && (tileID + 1) % int(gridSize.X) != 0 && tileID + 1 != tempRow * gridSize.X && IsTileValid(currentNeighbour))
	{
		neighboursAll.Add(currentNeighbour);
		neighbours.Add(currentNeighbour);
	}
	else if (allowDiagonalPaths)
	{
		neighboursAll.Add(-1);
	}

	// Top tile
	currentNeighbour = tileID + 1;
	tempRow = FMath::TruncToInt(float(tileID + 1 / gridSize.X));
	if ((tileID + 1) % int(gridSize.X) != 0 && IsTileValid(currentNeighbour) && tileID + 1 != tempRow * gridSize.X) 
	{
		neighboursAll.Add(currentNeighbour);
		neighbours.Add(currentNeighbour);
	}
	else
	{
		neighboursAll.Add(-1);
	}

	// Top right tile
	currentNeighbour = tileID - gridSize.X + 1;
	tempRow = FMath::TruncToInt(float(tileID + 1 / gridSize.X));
	if (allowDiagonalPaths && (tileID + 1) % int(gridSize.X) != 0 && tileID + 1 != tempRow * gridSize.X && IsTileValid(currentNeighbour))
	{
		neighboursAll.Add(currentNeighbour);
		neighbours.Add(currentNeighbour);
	}
	else if (allowDiagonalPaths)
	{
		neighboursAll.Add(-1);
	}

	AllNeighboursTileID = neighboursAll;
	return neighbours;
}

TArray<int> ATT_GridManager::GetTileNeighbours(int tileID, bool allowDiagonalPaths)
{
	TArray<int> tempBoolArray;
	TArray<int> returnArray = GetTileNeighbours(tileID, allowDiagonalPaths, tempBoolArray);

	return returnArray;
}

float ATT_GridManager::GetDistanceBetweenTiles()
{
	return distanceBetweenTiles;
}

FVector2D ATT_GridManager::GetGridSize()
{
	return FVector2D(gridSizeX, gridSizeY);
}


/*---------- Tile Spawning ----------*/

void ATT_GridManager::SpawnTiles(int x, int y, FVector center, float distance)
{
	int tileCounter = -1;

	for (int i = 0; i < y; i++) 
	{
		for (int j = 0; j < x; j++) 
		{
			tileCounter++;

			//Calculating new location (will spawn around center vector hence the very long expression). Taken from "2D Grid Execution Macro".
			FVector newLocation = FVector(center + (FVector(distance * 0.5f, distance * 0.5f, 0.0f) + (distance * (FVector((j - (float(x) / 2)), (i - (float(y) / 2)), 0.0f)))));
			tileLocations.Add(newLocation);

			FTransform tileTransform = FTransform(FRotator(0, 0, -90), newLocation, FVector(1, 1, 1));
			instanceGroupedSpriteComp->AddInstance(tileTransform, tileSpriteNormal, true, FLinearColor::White);

			if (displayTileID)
			{
				int tileID = tileCounter;
				ATextRenderActor* Text = GetWorld()->SpawnActor<ATextRenderActor>(ATextRenderActor::StaticClass(), FVector(0.f, 100, 170.f), FRotator(90.f, 180.f, 0.f));
				Text->GetTextRender()->SetText(FString::FromInt(tileID));
				Text->GetTextRender()->SetTextRenderColor(FColor::White);
				Text->GetTextRender()->SetVerticalAlignment(EVRTA_TextCenter);
				Text->GetTextRender()->SetHorizontalAlignment(EHTA_Center);
				Text->SetActorLocation(newLocation);
				Text->SetActorScale3D(FVector(4.f, 4.f, 4.f));

				tileIDActors.Add(Text);
			}

		}
	}

	for (int i = 0; i < x*y; i++)
	{
		tileIDs.Add(i);
	}
}

void ATT_GridManager::SpawnBlockManager()
{
		//Spawn TT_BlockManager and assign its TT_GridManager to this object
		ATT_BlockManager* newBlockManager;
		newBlockManager = GetWorld()->SpawnActorDeferred<ATT_BlockManager>(ATT_BlockManager::StaticClass(), GetActorTransform());
		
		if (newBlockManager) 
		{
			newBlockManager->SetGridManager(this);
			BlockManager = newBlockManager;

			UGameplayStatics::FinishSpawningActor(newBlockManager, GetActorTransform());
		}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("BlockManagerClass no set in GridManager. Please set it, and try again."))
	}
}

void ATT_GridManager::FetchZoneColours()
{
	for (auto i : BlockManager->zoneViewModeIndex)
	{
		zoneColours.Add(BlockManager->GetBlockStatsFromBlockID(i)->Grid_Colour);
	}	
}

void ATT_GridManager::OnTileHovered_Implementation(int tileID)
{
	// Check if the tile is already hovered
	if (!modifiedTiles.Contains(tileID) || viewModeTiles.Contains(tileID))
	{
		TileClearState();
		lastClickedTile = -1;

		FTransform tempTileTransform;
		instanceGroupedSpriteComp->GetInstanceTransform(tileID, tempTileTransform, true);
		FTransform newTransform = FTransform(tempTileTransform.GetRotation(), tempTileTransform.GetLocation(), FVector(1.1f, 1.1f, 1.1f));

		instanceGroupedSpriteComp->UpdateInstanceTransform(tileID, newTransform, true);

		//Marks the tile as hovered or "modified"
		modifiedTiles.Add(tileID);
	}
}

void ATT_GridManager::OnTileClicked_Implementation(int tileID)
{
	//Check the tile is hovered and hasn't been clicked
	if (modifiedTiles.Contains(tileID) && (lastClickedTile == -1))
	{
		TileClearState();

		FTransform tempTileTransform;
		instanceGroupedSpriteComp->GetInstanceTransform(tileID, tempTileTransform, true);
		FTransform newTransform = FTransform(tempTileTransform.GetRotation(), tempTileTransform.GetLocation(), FVector(0.8f, 0.8f, 0.8f));

		instanceGroupedSpriteComp->UpdateInstanceTransform(tileID, newTransform, true);

		//Marks the tile as hovered or "modified" and as clicked
		int32 clickedTile = tileID;
		modifiedTiles.Add(clickedTile);
	}
}


/*---------- Tile functions ----------*/

void ATT_GridManager::TileReset(int tileID)
{
	FTransform tempTileTransform;
	instanceGroupedSpriteComp->GetInstanceTransform(tileID, tempTileTransform, true);
	FTransform newTransform = FTransform(tempTileTransform.GetRotation(), tempTileTransform.GetLocation(), FVector(1.0f, 1.0f, 1.0f));

	instanceGroupedSpriteComp->UpdateInstanceTransform(tileID, newTransform, true);
	instanceGroupedSpriteComp->UpdateInstanceColor(tileID, FLinearColor::White, true);
}

void ATT_GridManager::SetTileColorFromZoneID(TArray<int> zoneTileIDs, int zoneID)
{
	if (zoneTileIDs.Num() > 0)
	{
		TileClearState();

		FLinearColor ZoneColour;
		ZoneColour = FVector(0.1, 0.1, 0.1);

		if(zoneID != -1)
		{ 
			ZoneColour = BlockManager->GetBlockStatsFromBlockID(zoneID)->Grid_Colour;
		}

		for (int i = 0; i < zoneTileIDs.Num(); ++i)
		{
				SetTileColor(zoneTileIDs[i], ZoneColour);
		}
	}
}

void ATT_GridManager::SetTileColor(int tileID, FLinearColor colour)
{
	if (IsTileValid(tileID))
	{
		instanceGroupedSpriteComp->UpdateInstanceColor(tileID, colour);
		modifiedTiles.Add(tileID);
	}
}

void ATT_GridManager::SetTileTransform(int tileID, FTransform newTransform)
{
	if (IsTileValid(tileID))
	{
		instanceGroupedSpriteComp->UpdateInstanceTransform(tileID, newTransform);
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("Tile ID was not valid, couldn't update tile transform."));
	}
}

void ATT_GridManager::TileClearState()
{
	if (modifiedTiles.Num() > 0) 
	{
			for (int i = 0; i < modifiedTiles.Num(); ++i)
			{
				int TileID = modifiedTiles[i];

				FTransform tempTileTransform;
				instanceGroupedSpriteComp->GetInstanceTransform(TileID, tempTileTransform, true);
				FTransform newTransform = FTransform(tempTileTransform.GetRotation(), tempTileTransform.GetLocation(), FVector(1.0f, 1.0f, 1.0f));

				instanceGroupedSpriteComp->UpdateInstanceTransform(TileID, newTransform, true);

				if (!viewModeTiles.Contains(TileID))
				{
					instanceGroupedSpriteComp->UpdateInstanceColor(TileID, FLinearColor::White, true);
				}
			}
			modifiedTiles.Empty();
	}

	// If no view modes active, reset all tiles that were affected 
	if (!isViewMode)
	{
		for (int i = 0; i < viewModeTiles.Num(); i++)
		{
			instanceGroupedSpriteComp->UpdateInstanceColor(viewModeTiles[i], FLinearColor::White, true);
		}
	}
}


/*---------- View modes functions ----------*/

void ATT_GridManager::ActivateZoneViewMode(int viewMode)
{
	if (!BlockManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("BlockManager hasn't been spawned from GridManager, cannot use view modes."));
		return;
	}

	// Stop any active ViewModes & Refresh zoneColour from data table
	StopZoneViewMode();
	FetchZoneColours();

	if (viewMode < BlockManager->isZoneViewModeActive.Num())
	{
		// Activate new one
		BlockManager->isZoneViewModeActive[viewMode] = true;
		isViewMode = true;

		// Start ViewModeTick Timer
		GetWorldTimerManager().SetTimer(TimerHandler_ViewMode, this, &ATT_GridManager::ViewModeTick, 0.1f, true, 0.0f);
	}
}

void ATT_GridManager::StopZoneViewMode()
{
	// Stop ViewModeTick timer
	GetWorldTimerManager().ClearTimer(TimerHandler_ViewMode);

	// Deactivate all view modes
	for (int i = 0; i < BlockManager->isZoneViewModeActive.Num(); i++)
	{
		BlockManager->isZoneViewModeActive[i] = false;
	}

	isViewMode = false;
	TileClearState();
}

void ATT_GridManager::ViewModeTick()
{
	if (!isViewMode)
	{
		return;
	}

	TArray<int> tempZoneTileIDs;
	tempZoneTileIDs = BlockManager->GetSpawnedZoneTileIDs();
	viewModeTiles.Empty();

	for (int i = 0; i < tempZoneTileIDs.Num(); i++)
	{
		if (!playerTileSelection.Contains(i))
		{
			// Check which mode is activated
			for (int viewModeIndex = 0; viewModeIndex < BlockManager->isZoneViewModeActive.Num(); viewModeIndex++)
			{
				if (BlockManager->isZoneViewModeActive[viewModeIndex])
				{
					// Check if the zone on the tile corresponds to the active view mode
					if (tempZoneTileIDs[i] == BlockManager->zoneViewModeIndex[viewModeIndex])
					{
						UE_LOG(LogTemp, Warning, TEXT("Yo road"));
						// Add the tile to view mode tile, & change its colour
						viewModeTiles.Add(i);
						SetTileColor(i, zoneColours[viewModeIndex]);
					}
				}
			}
		}
	}
}

void ATT_GridManager::SetPlayerSelection(TArray<int> selectedTileIDs)
{
	playerTileSelection.Empty();
	playerTileSelection = selectedTileIDs;
}

void ATT_GridManager::ClearPlayerSelection()
{
	for (auto i : playerTileSelection)
	{
		TileReset(i);
	}
	playerTileSelection.Empty();
}

bool ATT_GridManager::IsTileValid(int tileID)
{
	FTransform emptyTransform;
	return instanceGroupedSpriteComp->GetInstanceTransform(tileID, emptyTransform, false);
}

TArray<int> ATT_GridManager::GetAllTileIDs()
{
	return tileIDs;
}


/*---------- BlockManager Access -----------*/

ATT_BlockManager* ATT_GridManager::GetBlockManager()
{
	return BlockManager;
}

TArray<int> ATT_GridManager::GetZoneTileIDsFromZoneParameters(int tileA, int tileB, bool excludeTileB)
{
	return BlockManager->GetZoneTileIDsFromZoneParameters(tileA, tileB, excludeTileB);
}

int ATT_GridManager::GetZoneTileAFromHoveredTile(int tileC, int sizeX, int sizeY)
{
	return BlockManager->GetZoneStartTileFromHoveredTile(tileC, sizeX, sizeY, false);
}

int ATT_GridManager::GetZoneTileBFromZoneSize(int tileA, int sizeX, int sizeY)
{
	return BlockManager->GetZoneEndTileFromZoneSize(tileA, sizeX, sizeY, false);
}

FVector2D ATT_GridManager::GetZoneSizeFromTileArray(TArray<int> zone)
{
	return BlockManager->GetZoneSizeFromTileArray(zone);
}


