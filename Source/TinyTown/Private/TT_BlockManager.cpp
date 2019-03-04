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

	GetBlockDataTable();
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
	GridManager = newGridManager;
	SetArraySizes();
}

void ATT_BlockManager::SetArraySizes()
{
	int tileAmount = GridManager->GetGridSize().X * GridManager->GetGridSize().Y;

	spawnedBlockID.SetNum(tileAmount, true);
	spawnedBlocks.SetNum(tileAmount, true);
}


/*---------- Block building functions ----------*/

void ATT_BlockManager::CreateBlockOnTile(int TileID, int blockID, FRotator BlockRotation)
{
	if (!GridManager) 
	{
		UE_LOG(LogTemp, Log, TEXT("Grid Maker not set in %s, cannot build"), *GetName());
		return;
	}
		SpawnBlock(blockID, BlockRotation, TileID);
}

void ATT_BlockManager::CreateBlockOnTile(int TileID, FRotator BlockRotation, FString buildingType, int efficiency, int sizeX, int sizeY)
{
	if (!GridManager)
	{
		UE_LOG(LogTemp, Log, TEXT("Grid Maker not set in %s, cannot build"), *GetName());
		return;
	}

	int blockID = GetRandomBlockIDFromParameter(buildingType, efficiency, sizeX, sizeY);
	
	SpawnBlock(blockID, BlockRotation, TileID);
}

void ATT_BlockManager::SpawnBlock(int BlockID, FRotator BlockRotation, int TileID)
{
	// Makes block transform based on TileID
	FTransform BlockTransform = FTransform(FRotator(0, 0, 0), GridManager->GetTileLocation(TileID), FVector(1, 1, 1));

	ATT_Block* SpawnedActor;
	SpawnedActor = GetWorld()->SpawnActorDeferred<ATT_Block>(BlockToSpawn, BlockTransform);
	if (SpawnedActor)
	{
		//Get Block Default stats from data table
		FTT_Struct_Block* BlockStats = GetBlockStatsFromBlockID(BlockID);

		//Get the block's zone characteristics
		bool isModuloHalfPi = FMath::IsNearlyEqual(abs(BlockRotation.Yaw), 90, 0.1f);
		int BlockStartTile = GetZoneStartTileFromAnchorPoint(TileID, BlockStats->AnchorTileX, BlockStats->Size_X, BlockStats->Size_Y);
		int BlockEndTile = GetZoneEndTile (BlockStartTile, BlockStats->Size_X, BlockStats->Size_Y, isModuloHalfPi);
		TArray<int> BlockZoneTileIDs = CalculateZoneTileIDs(BlockStartTile, BlockEndTile);

		SpawnedActor->SetBlockStats(BlockStats);
		SpawnedActor->SetBlockManager(this);
		SpawnedActor->SetBlockTileIDs(BlockZoneTileIDs);
		SpawnedActor->SetBlockRotation(BlockRotation);
		SpawnedActor->SetBlockPosition();
		SpawnedActor->UpdateBlockRotationAndLocation();

		UGameplayStatics::FinishSpawningActor(SpawnedActor, BlockTransform);


		for (int i = 0; i < BlockZoneTileIDs.Num(); i++)
		{
			spawnedBlockID[BlockZoneTileIDs[i]] = BlockID;
			spawnedBlocks[BlockZoneTileIDs[i]] = SpawnedActor;
		}
	}
}

void ATT_BlockManager::ClearTileArraysAtIndex(int index)
{
	spawnedBlockID[index] = 0;
	spawnedBlocks[index] = nullptr;
}

void ATT_BlockManager::DeleteBlockOnTile(int TileID)
{
	if (!spawnedBlocks[TileID])
	{
		UE_LOG(LogTemp, Warning, TEXT("No block to delete at this TileID in BlockManager."));
		return;
	}

	ATT_Block* blockToDelete = spawnedBlocks[TileID];
	for (int i = 0; i < blockToDelete->GetBlockTileIDs().Num(); i++)
	{
		int indexToClear = blockToDelete->GetBlockTileIDs()[i];
		ClearTileArraysAtIndex(indexToClear);
	}

	blockToDelete->DestroyBlock();
}

TArray<int> ATT_BlockManager::CalculateZoneTileIDs(int StartTile, int EndTile)
{
	TArray<int> TileIDs;
	UE_LOG(LogTemp, Log, TEXT("CalculatingZoneTileIDs in BlockManager."));

	if (StartTile != 0 && EndTile != 0)
	{
		// Convert TileID into polar coordinates
		int Ay;
		int Ax;
		Ay = StartTile / GridManager->GetGridSize().X;
		Ax = StartTile - (Ay * GridManager->GetGridSize().X);

		int By;
		int Bx;
		By = EndTile / GridManager->GetGridSize().X;
		Bx = EndTile - (By * GridManager->GetGridSize().X);

		// Get block size from vector AB>
		FVector2D blockSize;

		blockSize = FVector2D(Bx - Ax, By - Ay);


		// Convert polar coordinates into grid of tiles
		float distance = GridManager->GetDistanceBetweenTiles();
		FVector startLocation = GridManager->GetTileLocation(StartTile);

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
				int newTileID = StartTile + j * xSign + (i * ySign  * GridManager->GetGridSize().X);
				
				TileIDs.Add(newTileID);
			}
		}
		GridManager->TileZoneRes(TileIDs);
		return TileIDs;
	}

	return TileIDs;
}

int ATT_BlockManager::GetZoneStartTileFromAnchorPoint(int AnchorTileID, int AnchorPointX, int SizeX, int SizeY)
{
	if (AnchorPointX == 0)
	{
		return AnchorTileID;
	}
	
	int anchorX;
	int anchorY;

	if (SizeX % 2 == 0)
	{
		anchorX = SizeX / 2;
	}
	else
	{
		anchorX = FMath::TruncToInt(SizeX / 2);
	}

	if (SizeY % 2 == 0)
	{
		anchorY = SizeY / 2;
	}
	else
	{
		anchorY = FMath::TruncToInt(SizeY / 2);
	}

	int AnchorRelativeTileID = anchorY + anchorX * SizeX;

	int ActualTileID = AnchorRelativeTileID - GridManager->GetGridSize().X - 1;

	int newStartTile = ActualTileID - (AnchorPointX * GridManager->GetGridSize().X);
	return newStartTile;
}

int ATT_BlockManager::GetZoneEndTile(int StartTile, int SizeX, int SizeY, bool isModuloHalfPi)
{
	int xSize = SizeX;
	int ySize = SizeY;

	if (isModuloHalfPi)
	{
		int tempXSize = xSize;
		xSize = ySize;
		ySize = tempXSize;
	}

	int newTileID = StartTile + (ySize * GridManager->GetGridSize().X) + xSize;
	return newTileID;
}


/*---------- Data Table functions ----------*/

void ATT_BlockManager::GetBlockDataTable()
{
	//Gets Data table
	static ConstructorHelpers::FObjectFinder<UDataTable> DataBlock_DataObject(TEXT("DataTable'/Game/Data/Data_Block.Data_Block'"));
	if (DataBlock_DataObject.Succeeded())
	{
		data_Block = DataBlock_DataObject.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find Data_Block in BlockManager."))
	}
}

FTT_Struct_Block* ATT_BlockManager::GetBlockStatsFromBlockID(int buildingID)
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
				if (name == FName(*FString::FromInt(buildingID)))
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

