// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_Block.h"
#include "Engine/DataTable.h"
#include "TT_Global.h"
#include "TT_BlockManager.h"
#include "TT_GridManager.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"


/*---------- Primary functions ----------*/

ATT_Block::ATT_Block()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	BuildingRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BuildingRoot"));
	BuildingRoot->SetupAttachment(Root);

	RotationRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RotationRoot"));
	RotationRoot->SetupAttachment(BuildingRoot);

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	BlockMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BlockMesh->SetupAttachment(RotationRoot);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	BoxComp->InitBoxExtent(FVector(50, 50, 100));
	BoxComp->SetupAttachment(RotationRoot);

	BoxComp->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	BoxComp->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Overlap);
	   
}

void ATT_Block::BeginPlay()
{
	Super::BeginPlay();


}


/*---------- Initialisation ----------*/

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

const FTT_Struct_Block ATT_Block::GetBlockData()
{
	return blockStats;
}

void ATT_Block::SetBlockManager(ATT_BlockManager* BlockManager)
{
	blockManager = BlockManager;
	distanceBetweenTiles = blockManager->GridManager->GetDistanceBetweenTiles();
}

ATT_BlockManager* ATT_Block::GetBlockManager()
{
	return blockManager;
}

ATT_GridManager* ATT_Block::GetGridManager()
{
	if (blockManager)
	{
		return blockManager->GridManager;
	}
	else
	{
		return nullptr;
	}
}

void ATT_Block::SetCentralTileID(int tileID)
{
	centralTileID = tileID;
}

void ATT_Block::SetBlockTileIDs(TArray<int> TileIDs)
{
	blockTileIDs = TileIDs;
}

TArray<int> ATT_Block::GetBlockTileIDs()
{
	return blockTileIDs;
}

void ATT_Block::GetOccupiedTileIDs(int32& centralTile, TArray<int32>& tileZone)
{
	centralTile = centralTileID;
	tileZone = blockTileIDs;
}

// Block positioning

void ATT_Block::SetBlockPosition()
{
	if (&blockStats == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Blockstats not valid, cannot adjust AnchorPosition. This check is based on block's name in the data table, if empty this error will always trigger."));
		return;
	}
	
	if (distanceBetweenTiles == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("distanceBetweenTiles = 0, block relative location won't be adjusted. Check if blockManager was set."))
		return;
	}
	
	/*
	The blocks have a relative location of 0,0,0 which follows the mouse cursor. This means that: 
	- If the block has an even XSize and YSize, the block will need to be re-ajusted to appear aligned on the grid.
	- On the other hand if the block has uneven XSize and YSize, the block will naturally appear centered since its zone has a central tile (RelativeLocation = 0,0,0).
	*/
	
	int newX;
	int newY;

	if (blockStats.Size_X % 2 == 0)
	{
		newX = distanceBetweenTiles / 2;
	}
	else
	{
		newX = 0;
	}

	if (blockStats.Size_Y % 2 == 0)
	{
		newY = distanceBetweenTiles / 2;
	}
	else
	{
		newY = 0;
	}

	if (FMath::IsNearlyEqual(abs(blockRotation.Yaw), 90, 0.1f))
	{
		blockAnchorLocation = FVector(newY, newX, 0);
		return;
	}

	blockAnchorLocation = FVector(newX, newY, 0);
}

void ATT_Block::SetBlockRotation(FRotator Rotation, float blockRotaSpeed)
{
	blockRotation = Rotation;
	blockRotationSpeed = blockRotaSpeed;
}

void ATT_Block::UpdateBlockRotationAndLocation()
{
	RotationRoot->SetRelativeRotation(blockRotation);
	BuildingRoot->SetRelativeLocation(blockAnchorLocation);
	
}

// Edit Mode

void ATT_Block::ActivateEditMode()
{
	isInEditingMode = true;

	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().SetTimer(TimerHandle_EditMode, this, &ATT_Block::EditModeTick, 0.01f, true, 0.0f);
}

void ATT_Block::StopEditMode()
{
	isInEditingMode = false;

	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GetWorldTimerManager().ClearTimer(TimerHandle_EditMode);
}

void ATT_Block::EditModeTick()
{
	// Updates location on tick (to ensure the block is always aligned on the grid regardless of its rotation)
	SetBlockPosition();

	BuildingRoot->SetRelativeLocation(FMath::Lerp(BuildingRoot->RelativeLocation, blockAnchorLocation, 20 * 0.01f));
	RotationRoot->SetRelativeRotation(FMath::Lerp(RotationRoot->RelativeRotation, blockRotation, 15 * blockRotationSpeed));
}

/*---------- Block Actions ----------*/

void ATT_Block::OnDestroyBlock_Implementation()
{
	this->Destroy();
}

