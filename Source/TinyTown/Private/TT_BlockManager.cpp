// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_BlockManager.h"
#include "TT_GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "PaperGroupedSpriteComponent.h"
#include "Engine/DataTable.h"
#include "TT_Global.h"
#include "TT_Block.h"

/*---------- Primary functions ----------*/

ATT_BlockManager::ATT_BlockManager()
{
	PrimaryActorTick.bCanEverTick = false;


	// Get the data table holding block's data
	data_Block = GetBlockDataTable();
	AnalyseDataBase();
}

void ATT_BlockManager::BeginPlay()
{
	Super::BeginPlay();
}

void ATT_BlockManager::SetGridManager(ATT_GridManager* newGridManager)
{
	if (!newGridManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't set new grid manager in block manager, null input."))
	}

	// If valid, set the grid manager
	GridManager = newGridManager;

	// Make sure tile arrays are resized
	int numberOfTiles = GridManager->GetGridSize().X * GridManager->GetGridSize().Y;
	SetTileArraysSize(numberOfTiles);
}

void ATT_BlockManager::SetTileArraysSize(int newArraySize)
{
	spawnedBlockID.SetNum(newArraySize, true);
	spawnedBlocks.SetNum(newArraySize, true);
	spawnedZoneID.SetNum(newArraySize, true);
}

TArray<int> ATT_BlockManager::GetSpawnedZoneTileIDs()
{
	return spawnedZoneID;
}


/*---------- Block & Zone building functions ----------*/

void ATT_BlockManager::SpawnBlockFromParameters(int tileID, FRotator blockRotation, FString buildingType, int efficiency, int sizeX, int sizeY)
{
	if (!GridManager)
	{
		UE_LOG(LogTemp, Log, TEXT("Grid Maker not set in %s, cannot build"), *GetName());
		return;
	}

	int blockID = GetRandomBlockIDFromParameter(buildingType, efficiency, sizeX, sizeY);
	
	SpawnBlock(blockID, blockRotation, tileID);
}

void ATT_BlockManager::SpawnBlock(int blockID, FRotator blockRotation, int tileID)
{
	// Makes block transform based on TileID
	FTransform BlockTransform = FTransform(FRotator(0, 0, 0), GridManager->GetTileLocation(tileID), FVector(1, 1, 1));

	//Get Block Default stats from data table
	FTT_Struct_Block* BlockStats = GetBlockStatsFromBlockID(blockID);

	//Get the block's zone characteristics
	bool isModuloHalfPi = FMath::IsNearlyEqual(abs(blockRotation.Yaw), 90, 0.1f);
	int BlockStartTile = GetZoneStartTileFromHoveredTile(tileID, BlockStats->Size_X, BlockStats->Size_Y, isModuloHalfPi);
	int BlockEndTile = GetZoneEndTileFromZoneSize(BlockStartTile, BlockStats->Size_X, BlockStats->Size_Y, isModuloHalfPi);
	TArray<int> BlockZoneTileIDs = GetZoneTileIDsFromZoneParameters(BlockStartTile, BlockEndTile);

	for (auto i : BlockZoneTileIDs)
	{
		if (spawnedBlockID[i] != 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("A block is already placed on one of these tiles."));
			return;
		}
	}

	ATT_Block* SpawnedActor;
	SpawnedActor = GetWorld()->SpawnActorDeferred<ATT_Block>(BlockToSpawn, BlockTransform);
	if (SpawnedActor)
	{
		SpawnedActor->SetBlockStats(BlockStats);
		SpawnedActor->SetBlockManager(this);
		SpawnedActor->SetBlockTileIDs(BlockZoneTileIDs);
		SpawnedActor->SetBlockRotation(blockRotation);
		SpawnedActor->SetBlockPosition();
		SpawnedActor->UpdateBlockRotationAndLocation();

		UGameplayStatics::FinishSpawningActor(SpawnedActor, BlockTransform);


		for (int i = 0; i < BlockZoneTileIDs.Num(); i++)
		{
			spawnedBlockID[BlockZoneTileIDs[i]] = blockID;
			spawnedBlocks[BlockZoneTileIDs[i]] = SpawnedActor;
		}
	}
}

void ATT_BlockManager::SpawnBlockAtStartTile(int blockID, int tileID)
{
	// Makes block transform based on TileID
	FTransform BlockTransform = FTransform(FRotator(0, 0, 0), GridManager->GetTileLocation(tileID), FVector(1, 1, 1));

	//Get Block Default stats from data table
	FTT_Struct_Block* BlockStats = GetBlockStatsFromBlockID(blockID);

	//Get the block's zone characteristics
	int BlockStartTile = tileID;
	int BlockEndTile = GetZoneEndTileFromZoneSize(BlockStartTile, BlockStats->Size_X, BlockStats->Size_Y, false);
	TArray<int> BlockZoneTileIDs = GetZoneTileIDsFromZoneParameters(BlockStartTile, BlockEndTile);

	for (auto i : BlockZoneTileIDs)
	{
		if (spawnedBlockID[i] != 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("A block is already placed on one of these tiles."));
			return;
		}
	}

	ATT_Block* SpawnedActor;
	SpawnedActor = GetWorld()->SpawnActorDeferred<ATT_Block>(BlockToSpawn, BlockTransform);
	if (SpawnedActor)
	{
		SpawnedActor->SetBlockStats(BlockStats);
		SpawnedActor->SetBlockManager(this);
		SpawnedActor->SetBlockTileIDs(BlockZoneTileIDs);
		SpawnedActor->SetBlockPosition();
		SpawnedActor->UpdateBlockRotationAndLocation();

		UGameplayStatics::FinishSpawningActor(SpawnedActor, BlockTransform);


		for (int i = 0; i < BlockZoneTileIDs.Num(); i++)
		{
			spawnedBlockID[BlockZoneTileIDs[i]] = blockID;
			spawnedBlocks[BlockZoneTileIDs[i]] = SpawnedActor;
		}
	}
}

void ATT_BlockManager::DeleteBlockOnTile(int tileID)
{
	if (!spawnedBlocks[tileID])
	{
		DeleteZoneOnTile(tileID);
		return;
	}

	ATT_Block* blockToDelete = spawnedBlocks[tileID];
	for (int i = 0; i < blockToDelete->GetBlockTileIDs().Num(); i++)
	{
		int indexToClear = blockToDelete->GetBlockTileIDs()[i];
		ClearTileArraysAtIndex(indexToClear);
	}

	blockToDelete->DestroyBlock();
}

void ATT_BlockManager::CreateZoneOnTiles(TArray<int> tileIDs, int zoneID)
{
	for (int i = 0; i < tileIDs.Num(); i++)
	{
		spawnedZoneID[tileIDs[i]] = zoneID;
	}
	FindZoneLayout(zoneID, tileIDs);
}

void ATT_BlockManager::DeleteZoneOnTile(int tileID)
{
	if (spawnedZoneID[tileID] == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No blocks nor zones to delete at this TileID in BlockManager."));
		return;
	}

	spawnedZoneID[tileID] = 0;

}

void ATT_BlockManager::ClearTileArraysAtIndex(int index)
{
	spawnedBlockID[index] = 0;
	spawnedBlocks[index] = nullptr;
}

void ATT_BlockManager::FindZoneLayout(int zoneID, TArray<int> zone)
{
	// Gets the zone's size
	FVector2D zoneSize = GetZoneSizeFromTileArray(zone);

	UE_LOG(LogTemp, Error, TEXT("%s"), *zoneSize.ToString());

	if (zoneSize.X <= 0 && zoneSize.Y <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("It seems the zone's size can't be found in FindZoneLayout."));
		return;
	}

	int randTile = FMath::RandRange(0, zone.Num() - 1);



	// Finds all the possible block sizes in that space
	TArray<FVector2D> placeableSizes;
	for (int i = 1; i <= zoneSize.X; i++)
	{
		for (int j = 1; j <= zoneSize.Y; j++)
		{
			placeableSizes.Add(FVector2D(i, j));
		}
	}
	for (int i = 0; i < placeableSizes.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *placeableSizes[i].ToString());
	}

	// Gets placeable block from random size
	FString zoneType = GetBlockStatsFromBlockID(zoneID)->Block_Name.ToString();
	TArray<int> placeableBlocks;
	while (placeableBlocks.Num() == 0)
	{
		int randomIndex = FMath::RandRange(0, placeableSizes.Num() - 1);
		placeableBlocks = GetAllBlockIDsFromParameter(zoneType, 1, placeableSizes[randomIndex].X, placeableSizes[randomIndex].Y);
	}

	// Place block
	SpawnBlockAtStartTile(placeableBlocks[0], zone[0]);
}

/*---------- Zone Tiles functions ----------*/

TArray<int> ATT_BlockManager::GetZoneTileIDsFromZoneParameters(int tileA, int tileB)
{
	TArray<int> TileIDs;
	UE_LOG(LogTemp, Log, TEXT("CalculatingZoneTileIDs in BlockManager."));

	if (tileA != 0 && tileB != 0)
	{
		// Convert TileID into polar coordinates
		int Ay;
		int Ax;
		Ay = tileA / GridManager->GetGridSize().X;
		Ax = tileA - (Ay * GridManager->GetGridSize().X);

		int By;
		int Bx;
		By = tileB / GridManager->GetGridSize().X;
		Bx = tileB - (By * GridManager->GetGridSize().X);

		// Get block size from vector AB>
		FVector2D blockSize;

		blockSize = FVector2D(Bx - Ax, By - Ay);


		// Convert polar coordinates into grid of tiles
		float distance = GridManager->GetDistanceBetweenTiles();
		FVector startLocation = GridManager->GetTileLocation(tileA);

		float xSign = 1.0f;
		float ySign = 1.0f;

		if (blockSize.X != 0)
		{
			xSign = abs(blockSize.X) / blockSize.X;
		}
		if (blockSize.Y != 0)
		{
			ySign = abs(blockSize.Y) / blockSize.Y;
		}


		for (int i = 0; i < abs(blockSize.Y); i++)
		{
			for (int j = 0; j < abs(blockSize.X); j++)
			{
				int newTileID = tileA + j * xSign + (i * ySign  * GridManager->GetGridSize().X);
				
				TileIDs.Add(newTileID);
			}
		}
		return TileIDs;
	}

	return TileIDs;
}

int ATT_BlockManager::GetZoneStartTileFromHoveredTile(int tileC, int sizeX, int sizeY, bool isModuloHalfPi)
{
	/* This function is tightly bound to the way a building is moved and rotated (when being placed down). 
		For any changes to this function, make sure to change EditMode in Block.cpp	*/

	int offsetX;
	int offsetY;

	if (sizeX % 2 == 0)
	{
		offsetX = FMath::TruncToInt(sizeX / 2) - 1;
	}
	else
	{
		offsetX = FMath::TruncToInt(sizeX / 2);
	}

	if (sizeY % 2 == 0)
	{
		offsetY = FMath::TruncToInt(sizeY / 2) - 1;
	}
	else
	{
		offsetY = FMath::TruncToInt(sizeY / 2);
	}

	int ActualTileID;

	// Is the block rotated 90°
	if (isModuloHalfPi)
	{
		ActualTileID = tileC - offsetX * GridManager->GetGridSize().X - offsetY;
		return ActualTileID;
	}

	ActualTileID = tileC - offsetY * GridManager->GetGridSize().X - offsetX;
	return ActualTileID;
}

int ATT_BlockManager::GetZoneEndTileFromZoneSize(int tileA, int sizeX, int sizeY, bool isModuloHalfPi)
{
	int xSize = sizeX;
	int ySize = sizeY;

	if (isModuloHalfPi)
	{
		int tempXSize = xSize;
		xSize = ySize;
		ySize = tempXSize;
	}

	int newTileID = tileA + (ySize * GridManager->GetGridSize().X) + xSize;
	return newTileID;
}

FVector2D ATT_BlockManager::GetZoneSizeFromTileArray(TArray<int> zone)
{
	int gridX = 0;
	int gridY = 0;

	int tempTileID = zone[0];
	gridX++;
	gridY++;

	for (auto i : zone)
	{
		bool xDone = false;

		if (!xDone)
		{
			if (i == tempTileID + 1 || i == tempTileID - 1)
			{
				tempTileID = i;
				gridX++;
			}
		}

		if (i == tempTileID + GridManager->GetGridSize().X || i == tempTileID - GridManager->GetGridSize().X)
		{
			xDone = true;
			tempTileID = i;
			gridY++;
		}
	}

	FVector2D Result;
	Result.X = gridX;
	Result.Y = gridY;

	return Result;
}

/*---------- Data Table functions ----------*/

UDataTable* ATT_BlockManager::GetBlockDataTable()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataBlock_DataObject(TEXT("DataTable'/Game/Data/Data_Block.Data_Block'"));
	if (DataBlock_DataObject.Succeeded())
	{		
		return DataBlock_DataObject.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find Data_Block in BlockManager."));
		return nullptr;
	}
}

void ATT_BlockManager::AnalyseDataBase()
{
	if (data_Block)
	{
		FString contextString;
		TArray<FName> rowNames;
		rowNames = data_Block->GetRowNames();

		for (auto& name : rowNames)
		{
			FTT_Struct_Block* row = data_Block->FindRow<FTT_Struct_Block>(name, contextString);
			if (row)
			{
				// If type hasn't been encountered yet
				if (!blockTypeMap.Contains(row->Type))
				{
					// Create a new map element for the new type, and add the first blockID to it
					FTT_Struct_BlockType tempBlockType
					(
						row->Type,
						TArray<int>({ FCString::Atoi(*name.ToString()) })
					);

					blockTypeMap.Add(row->Type, tempBlockType);
				}

				// If type already exist in the map
				else
				{
					// Just add blockID to it
					blockTypeMap.Find(row->Type)->BlockIDs.Add( FCString::Atoi(*name.ToString()) );
				}

				// If row is a zone add it to the zone map
				if (row->Type == "Zone" || row->Type == "Road")
				{
					zoneIDMap.Add(row->Block_Name.ToString(), FCString::Atoi(*name.ToString()) );
				}
			}
		}

		// Set grid manager values for zones
		zoneIDMap.GenerateValueArray(zoneViewModeIndex);
		isZoneViewModeActive.SetNum(zoneViewModeIndex.Num(), true);

		// Log all types once found.
		TArray<FString> mapKey;
		blockTypeMap.GetKeys(mapKey);
		for (int i = 0; i < mapKey.Num(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("Yo, I found the type: %s"), *mapKey[i]);
		}
	}
}

FTT_Struct_Block* ATT_BlockManager::GetBlockStatsFromBlockID(int blockID)
{
	if (data_Block)
	{
		FString contextString;
		TArray<FName> rowNames;
		rowNames = data_Block->GetRowNames();

		// For all rows of the data table
		for (auto& name : rowNames)
		{
			FTT_Struct_Block* row = data_Block->FindRow<FTT_Struct_Block>(name, contextString);
			if (row)
			{
				// Check if current row is row ID = buildingID
				if (name == FName(*FString::FromInt(blockID)))
				{
					return row;
				}
			}
		}
		UE_LOG(LogTemp, Log, TEXT("No block matching that buildingID was found."));
		return nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("Cannot access data_Block in %s , data_Block is not valid."), *GetName());
	return nullptr;
}

TArray<int> ATT_BlockManager::GetAllBlockIDsFromParameter(FString buildingType, int efficiency, int sizeX, int sizeY)
{
	if (!data_Block)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot access data_Block in /% , data_Block is not valid."), *GetName())
	}

	FString contextString;
	TArray<FName> rowNames;
	TArray<int> matchingBlocks;
	rowNames = data_Block->GetRowNames();

	// For all rows of the data table
	for (auto& name : rowNames)
	{
		FTT_Struct_Block* row = data_Block->FindRow<FTT_Struct_Block>(name, contextString);
		if (row)
		{
			// Check if current row matches parameters
			if (row->Type == buildingType && row->Size_X == sizeX && row->Size_Y == sizeY)
			{
				int BlockID = FCString::Atoi(*name.ToString());
				matchingBlocks.Add(BlockID);
				UE_LOG(LogTemp, Warning, TEXT("%d"), BlockID);
			}
		}
	}

	if (!(matchingBlocks.Num() == 0))
	{
		UE_LOG(LogTemp, Log, TEXT("No block matching parameters was found."));
	}

	return matchingBlocks;
}

int  ATT_BlockManager::GetRandomBlockIDFromParameter(FString buildingType, int efficiency, int sizeX, int sizeY)
{
	//Get array of matching blocks
	TArray<int> matchingBlocks;
	matchingBlocks = GetAllBlockIDsFromParameter(buildingType, efficiency, sizeX, sizeY);

	if (matchingBlocks.Num() > 0)
	{
		// Return random index in matchingBlock array
		int randomIndex = FMath::RandRange(0, (matchingBlocks.Num() - 1));
		return matchingBlocks[randomIndex];
	}

	return 0;
}

