// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_Block.h"
#include "Engine/DataTable.h"
#include "TT_Global.h"
#include "TT_BlockManager.h"


/*---------- Primary functions ----------*/

ATT_Block::ATT_Block()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	BuildingRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BlockRoot"));
	BuildingRoot->AddLocalOffset(FVector(-128, -128, 0), false);
	BuildingRoot->SetupAttachment(Root);
}

void ATT_Block::BeginPlay()
{
	Super::BeginPlay();
}

void ATT_Block::SetBlockStats(FTT_Struct_Block* inputStats)
{
	if (inputStats)
	{
		blockStats = *inputStats;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Block stats not set on its creation in %s. inputStats not valid."), *GetName());
	}
}

FTT_Struct_Block* ATT_Block::GetBlockStats()
{
	return &blockStats;
}

void ATT_Block::SetBlockManager(ATT_BlockManager* BlockManager)
{
	blockManager = BlockManager;
}

ATT_BlockManager* ATT_Block::GetBlockManager()
{
	return blockManager;
}

void ATT_Block::SetBlockTileIDs(TArray<int> TileIDs)
{
	blockTileIDs = TileIDs;
}

TArray<int> ATT_Block::GetBlockTileIDs()
{
	return blockTileIDs;
}

void ATT_Block::DestroyBlock()
{
	this->Destroy();
}

