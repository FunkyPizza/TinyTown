// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_PlayerGridCamera.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "TT_GridManager.h"
#include "TT_BlockManager.h"
#include "TT_Block.h"
#include "TimerManager.h"
#include "TT_Pathfinder.h"
#include "TT_Global.h"


ATT_PlayerGridCamera::ATT_PlayerGridCamera()
{	
/*---------- Components creation ----------*/
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);

	PathfinderComp = CreateDefaultSubobject<UTT_Pathfinder>(TEXT("PathfinderComp"));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);


/*---------- Setting defaults ----------*/
	PrimaryActorTick.bCanEverTick = true;

	lastLinetracedTile = 0;

}

void ATT_PlayerGridCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update SpringArmLength with a lerp
	if (SpringArmComp->TargetArmLength != newTargetArmLength)
	{
		SpringArmComp->TargetArmLength = FMath::Lerp(SpringArmComp->TargetArmLength, newTargetArmLength, zoomLerpSpeed * DeltaTime);
	}

	// Update placingBlockGhost location to follow mouse
	if (isPlacingDownABlock)
	{
		TickBuildTool(DeltaTime);
	}

	// Linetrace cursor to 3D world
	MouseTrace();
}

void ATT_PlayerGridCamera::BeginPlay()
{
	Super::BeginPlay();

	//Get the grid manager the player interacts with 
	GridManager = GetGridManager();

	//Enables camera movement
	isMovementEnabled = true;

	//Set camera zoom values
	zoomCurrentStep = zoomSteps / 2;
	zoomCoefficient = (MaxSpringArmLength - MinSpringArmLength) / zoomSteps;
	InputCameraZoom(1);
}

ATT_GridManager* ATT_PlayerGridCamera::GetGridManager()
{
	if (GridManager)
	{
		return GridManager;
	}

	else
	{
		// Iterate through actors of class TT_GridManager and gets first result. (There should be only one at all times)
		for (TActorIterator<ATT_GridManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr->IsA(ATT_GridManager::StaticClass()))
			{
				return *ActorItr;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Couldn't get GridManager in PlayerGridCamera."))
			return nullptr;
	}
}

ATT_BlockManager* ATT_PlayerGridCamera::GetBlockManager()
{
	if (GridManager)
	{
		return GridManager->BlockManager;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot retrieve Blockmanager from TT_PlayerGridCamera."))
		return nullptr;
	}
}


/*---------- Player input functions ----------*/

void ATT_PlayerGridCamera::InputCameraZoom(float value)
{
	if (isMovementEnabled && value != 0) 
	{
		// Zoom out
		if (value < -0.1f) {
			zoomCurrentStep = FMath::Clamp(zoomCurrentStep + 1, 1, zoomSteps);

			newTargetArmLength = zoomCurrentStep * zoomCoefficient + MinSpringArmLength;
		}

		// Zoom in
		if (value > 0.1f) 
		{
			zoomCurrentStep = FMath::Clamp(zoomCurrentStep - 1, 1, zoomSteps);

			newTargetArmLength = zoomCurrentStep * zoomCoefficient + MinSpringArmLength;
		}
	}
}

void ATT_PlayerGridCamera::InputSelectButtonDown()
{
	isSelectButtonDown = true;

	if (isRemoveToolActive)
	{
		ConfirmRemoveToolStartTile();
	}

	if (isPlacingDownABlock)
	{
		// Temporarily save mouse position (used to set mouse position back after building rotation)
		GetWorld()->GetFirstPlayerController()->GetMousePosition(placingBlockMouseX, placingBlockMouseY);

		// Activate zone building if player is placing down a resizable block
		if (isPlacingDownAResizableBlock && !isSettingBlockSize)
		{
			ConfirmBuildToolStartTile();
			return;
		}

		// If already activated, confirm zone size
		if (isSettingBlockSize)
		{
			isSettingBlockSize = false;
		}
	}

	if (currentLinetracedTile != -1) 
	{
		SelectBlockFromTileID(currentLinetracedTile);
		GridManager->OnTileClicked(currentLinetracedTile);
	}
}

void ATT_PlayerGridCamera::InputSelectButtonUp()
{
	isSelectButtonDown = false;

	if (currentLinetracedTile != -1)
	{
		GridManager->TileClearState();
		currentLinetracedTile = -1;
	}

	if (isRemoveToolActive)
	{
		ConfirmRemoveToolEndTile();
	}

	// Set mouse position back to where it was before block rotation and stop block rotation.
	if (isRotatingBlock)
	{
		GetWorld()->GetFirstPlayerController()->SetMouseLocation(placingBlockMouseX, placingBlockMouseY);
		isRotatingBlock = false;
		return;
	}

	// If not rotating block, confirm block building location.
	if (isPlacingDownABlock && !isZoneBuildingCancelled)
	{
		ConfirmBuildTool();
		return;
	}

	isZoneBuildingCancelled = false;
}

void ATT_PlayerGridCamera::InputCancel()
{
	if (isPlacingDownABlock)
	{
		isZoneBuildingCancelled = true;
		StopBuildTool();
	}

	if (isRemoveToolActive)
	{
		StopRemoveTool();
	}

	if (selectedBlock != nullptr)
	{
		DeselectBlock();
	}
}


/*---------- Mouse rotation functions ----------*/

void ATT_PlayerGridCamera::InputRotationButtonDown()
{
	isRotationButtonDown = true;
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MouseRotation, this, &ATT_PlayerGridCamera::MouseRotationTick, GetWorld()->DeltaTimeSeconds, true, 0.0f);
}

void ATT_PlayerGridCamera::MouseRotationTick()
{
	if (isMovementEnabled)
	{
		if (isRotationButtonDown)
		{
			isRotatingCamera = true;

			RotateCamera(GetInputAxisValue("MouseX"), GetInputAxisValue("MouseY"), mouseXRotationSpeed, mouseYRotationSpeed);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_MouseRotation, this, &ATT_PlayerGridCamera::MouseRotationTick, GetWorld()->DeltaTimeSeconds, true, 0.0f);
		}

		else
		{
			isRotatingCamera = false;
		}
	}
}

void ATT_PlayerGridCamera::InputRotationButtonUp()
{
	isRotationButtonDown = false;

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_MouseRotation);
}


/*---------- Mouse movements functions ----------*/

void ATT_PlayerGridCamera::InputMoveButtonDown()
{
	isMoveButtonDown = true;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MouseMovements, this, &ATT_PlayerGridCamera::MouseMovementsTick, GetWorld()->DeltaTimeSeconds, true, 0.0f);
}

void ATT_PlayerGridCamera::MouseMovementsTick()
{
	if (isMovementEnabled)
	{
		if (isMoveButtonDown)
		{
			isMovingCamera = true;

			MoveCamera(FMath::Clamp(-GetInputAxisValue("MouseX"), -1.0f, 1.0f), FMath::Clamp(-GetInputAxisValue("MouseY"), -1.0f, 1.0f), mouseMovementSpeed);
		}

		else
		{
			isMovingCamera = false;
		}
	}
}

void ATT_PlayerGridCamera::InputMoveButtonUp()
{
	isMoveButtonDown = false;

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_MouseMovements);
}


/*---------- Camera movement functions --------Z--*/

void ATT_PlayerGridCamera::MoveCamera(float x, float y, float sensitivity)
{
	if (isMovementEnabled)
	{
		isMovingCamera = true;

		//Avoids a speed increase when both X and Y axis = 1.
		float axisYNormalized = y * FMath::Abs(FMath::Sin(FMath::Atan2(y, x)));
		float axisXNormalized = x * FMath::Abs(FMath::Cos(FMath::Atan2(y, x)));

		FVector movementVector = FVector(axisYNormalized * sensitivity, axisXNormalized * sensitivity, 0.0f);

		AddActorLocalOffset(movementVector, false);
	}

	else
	{
		isMovingCamera = false;
	}
}

void ATT_PlayerGridCamera::RotateCamera(float x, float y, float xSensitivity, float ySensitivity)
{
	if (isMovementEnabled)
	{
		isRotatingCamera = true;

		// The camera's rotation values are negative, so we need the negative.
		float negativeMaxPitch = maximumPitch * -1;
		float negativeMinPitch = minimumPitch * -1;

		FRotator currentRotation = SpringArmComp->GetRelativeTransform().GetRotation().Rotator();

		// Clamps camera angle 
		if (currentRotation.Pitch >= negativeMinPitch)
		{
			SpringArmComp->SetRelativeRotation(FRotator(negativeMinPitch - 0.1f, 0, 0));
			return;
		}
		if (currentRotation.Pitch <= negativeMaxPitch)
		{
			SpringArmComp->SetRelativeRotation(FRotator(negativeMaxPitch + 0.1f, 0, 0));
			return;
		}

		// Rotate Y
		FRotator newRotationY = FRotator(y, 0, 0) * ySensitivity;
		SpringArmComp->AddLocalRotation(newRotationY,  false);

		// Rotate X
		FRotator newRotationX = FRotator(0, x, 0) * xSensitivity;
		AddActorLocalRotation(newRotationX, false);
	}

	else
	{
		isRotatingCamera = false;
	}

}

bool ATT_PlayerGridCamera::SelectBlockFromTileID(int tileID)
{
	if (GridManager->IsTileValid(tileID))
	{
		if (GetBlockManager()->spawnedBlocks[tileID] != nullptr)
		{
			selectedBlock = GetBlockManager()->spawnedBlocks[tileID];

			isBlockSelected = true;

			OnBlockSelected();

			return true;
		}
	}
	return false;
}

bool ATT_PlayerGridCamera::SelectBlock(ATT_Block* blockToSelect)
{
	if (blockToSelect)
	{
		selectedBlock = blockToSelect;
		isBlockSelected = true;

		OnBlockSelected();
		return true;
	}
	return false;
}

void ATT_PlayerGridCamera::DeselectBlock()
{
	if (selectedBlock != nullptr)
	{
		selectedBlock = nullptr;

		OnBlockDeselected();
	}
	isBlockSelected = false;
}

void ATT_PlayerGridCamera::MouseTrace()
{
	// Checks if there is a valid TT_GridManager and only line traces if the player hasn't clicked yet
	if (GridManager)
	{
		if (!isSelectButtonDown || isSettingBlockSize || isRemoveToolActive)
		{
			FHitResult Hit;
			if (GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, true, Hit))
			{
				if (Hit.Actor == GridManager)
				{
					if (Hit.Item != currentLinetracedTile)
					{
						// Hit a tile
						currentLinetracedTile = Hit.Item;
						lastLinetracedTile = currentLinetracedTile;

						if (!isSettingBlockSize)
						{
							GridManager->OnTileHovered(currentLinetracedTile);
						}
						return;
					}

				}

				if (Hit.Actor->GetClass()->IsChildOf(ATT_Block::StaticClass()))
				{
					currentLinetracedTile = Cast<ATT_Block>(Hit.Actor)->centralTileID;
					lastLinetracedTile = currentLinetracedTile;

					if (!isSettingBlockSize)
					{
						GridManager->OnTileHovered(currentLinetracedTile);
					}

					return;
				}
			}

			else
			{
				// No tile found
				GridManager->TileClearState();
				currentLinetracedTile = -1;
			}
		}
	}
}


/*---------- Build Tool / Building Blocks ----------*/

void ATT_PlayerGridCamera::StartBuildTool(int blockID)
{
	StopBuildTool();
	StopRemoveTool();

	if (currentBuildToolBlock)
	{
		// Cancel previous block building
		
	}

	// Placing block setting
	placingBlockID = blockID;
	FTT_Struct_Block* placingBlockStats = GridManager->BlockManager->GetBlockStatsFromBlockID(placingBlockID);
	isPlacingDownAResizableBlock = placingBlockStats->Block_Type == EBlockType::BT_Path || placingBlockStats->Block_Type == EBlockType::BT_Zone;
	isPlacingDownAPath = placingBlockStats->Block_Type == EBlockType::BT_Path;

	FTransform blockTransform = FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(1,1,1));
	placingBlockTargetLocation = FVector(0, 0, 0);
	placingBlockTargetRotation = FRotator(0, 0, 0);
	  
	// Spawning the placing block
	currentBuildToolBlock = GetWorld()->SpawnActorDeferred<ATT_Block>(placingBlockStats->BlockClass, blockTransform);

	if (currentBuildToolBlock)
	{
		currentBuildToolBlock->SetBlockStats(placingBlockStats);
		currentBuildToolBlock->SetBlockManager(GridManager->BlockManager);
		currentBuildToolBlock->ActivateEditMode();

		UGameplayStatics::FinishSpawningActor(currentBuildToolBlock, blockTransform);

		isPlacingDownABlock = true;
	}
}

void ATT_PlayerGridCamera::StopBuildTool()
{
	// Checks if the BuildTool should be reset instead of cancelled
	if (isZoneBuildingCancelled && isSettingBlockSize)
	{
		isSettingBlockSize = false;
		isMovementEnabled = true;
		GridManager->TileClearState();
		return;
	}

	// Makes sure the BuildTool is active, then fully disable it
	if (isPlacingDownABlock)
	{
		isPlacingDownABlock = false;
		isSettingBlockSize = false;
		isPlacingDownAResizableBlock = false;
		isZoneBuildingCancelled = false;
		GridManager->TileClearState();

		currentBuildToolBlock->Destroy();
		placingBlockID = -1;
		lastPathGoalTile = -1;

		isMovementEnabled = true;
	}
}

void ATT_PlayerGridCamera::ConfirmBuildTool()
{
	if (placingBlockID == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("the placing block ID was not set, cannot spawn block"));
		return;
	}
	if (!currentBuildToolBlock)
	{
		placingBlockID = -1;
		UE_LOG(LogTemp, Warning, TEXT("The placing block instance is not valid, cannot spawn block"));
		return;
	}

	// Block has a fixed size
	if (!isPlacingDownAResizableBlock)
	{
		GridManager->BlockManager->SpawnBlock(placingBlockID, placingBlockTargetRotation, lastBuildableTileID);
	}

	// Block is a zone / is resizable
	if (isPlacingDownAResizableBlock)
	{
		GridManager->BlockManager->CreateZoneOnTiles(placingLastZoneBuilt, placingBlockID);
		GridManager->ClearPlayerSelection();

		for (int i : placingLastZoneBuilt)
		{
			GridManager->TileReset(i);
		}
	}

	// Makes sure the build tool will be fully disabled
	isZoneBuildingCancelled = false;
	StopBuildTool();
}

void ATT_PlayerGridCamera::TickBuildTool(float deltaTime)
{
	if (currentBuildToolBlock)
	{
		if (isPlacingDownAResizableBlock)
		{
			TArray<int> tempTiles;
			tempTiles.Add(lastLinetracedTile);
			GridManager->SetTileColorToBlockID(tempTiles, placingBlockID);
		}

		// Enables keyboard movement when placing down a block
		if (!isSelectButtonDown)
		{
			isMovementEnabled = true;
		}

		// Resizing the block
		if (isSettingBlockSize && !isZoneBuildingCancelled)
		{
			if (isPlacingDownAPath)
			{
				if (lastPathGoalTile != lastLinetracedTile)
				{
					// Pathfinding functions
					//placingLastZoneBuilt = PathfinderComp->FindShortestPathInZoneDijkstra(placingBlockTileID, lastLinetracedTile, GetZoneTileIDsFromZoneParameters(placingBlockTileID, lastLinetracedTile));
					//placingLastZoneBuilt = PathfinderComp->FindShortestPathDijkstra(placingBlockTileID, lastLinetracedTile);
					//placingLastZoneBuilt = PathfinderComp->FindShortestPathAStar(placingBlockTileID, lastLinetracedTile);
					TArray<int> blocksToIgnore;
					blocksToIgnore.Add(placingBlockID); 

					placingLastZoneBuilt = PathfinderComp->FindShortestPathInZoneDijkstra(placingBlockTileID, lastLinetracedTile, GridManager->BlockManager->GetZoneTileIDsFromZoneParameters(placingBlockTileID, lastLinetracedTile, false), false, blocksToIgnore);

					if (placingLastZoneBuilt.Num() < 5)
					{
						placingLastZoneBuilt = PathfinderComp->FindShortestPathAStar(placingBlockTileID, lastLinetracedTile, false, blocksToIgnore);
					}

					lastPathGoalTile = lastLinetracedTile;
				}
					GridManager->SetPlayerSelection(placingLastZoneBuilt);
					GridManager->SetTileColorToBlockID(placingLastZoneBuilt, placingBlockID);
			}

			else 
			{
				placingLastZoneBuilt = GridManager->BlockManager->GetZoneTileIDsFromZoneParameters(placingBlockTileID, lastLinetracedTile, false);
				GridManager->SetPlayerSelection(placingLastZoneBuilt);
				GridManager->SetTileColorToBlockID(placingLastZoneBuilt, placingBlockID);

			}
		}

		// If not, rotate it
		else if (!isZoneBuildingCancelled)
		{
			// Check that the mouse button is held and that the block has finished its previous rotation
			if (isSelectButtonDown && FMath::IsNearlyEqual(currentBuildToolBlock->RotationRoot->RelativeRotation.Yaw, placingBlockTargetRotation.Yaw, 0.1f))
			{
				// Check for any mouse movement meaning the player wants to rotate the block
				if (!(FMath::IsNearlyEqual(GetInputAxisValue("MouseX"), 0.0f, blockRotationMouseThreshold)))
				{
					isRotatingBlock = true;
					isMovementEnabled = false;
				}

				if (GetInputAxisValue("MouseX") > blockRotationMouseThreshold)
				{
					placingBlockTargetRotation = placingBlockTargetRotation - FRotator(0, 90, 0);

					if (placingBlockTargetRotation.Yaw < -180)
					{
						placingBlockTargetRotation = FRotator(0, 90, 0);
					}
				}

				if (GetInputAxisValue("MouseX") < -blockRotationMouseThreshold)
				{
					isRotatingBlock = true;
					isMovementEnabled = false;

					placingBlockTargetRotation = placingBlockTargetRotation + FRotator(0, 90, 0);

					if (placingBlockTargetRotation.Yaw > 180)
					{
						placingBlockTargetRotation = FRotator(0, -90, 0);
					}
				}
			}
		}

		// Check if block is clear to be placed (not hovering used tiles)
		bool isBlockClearToBePlaced = true;
		bool isModuloHalfPi = FMath::IsNearlyEqual(abs(placingBlockTargetRotation.Yaw), 90, 0.1f);

		if (!isSettingBlockSize)
		{			
			isBlockClearToBePlaced = GridManager->BlockManager->CheckIfBlockIsBuildable
			(
				lastLinetracedTile, 
				currentBuildToolBlock->GetBlockStats()->Size_X, 
				currentBuildToolBlock->GetBlockStats()->Size_Y, 
				isModuloHalfPi
			);
		}

		if (isBlockClearToBePlaced)
		{
			lastBuildableTileID = lastLinetracedTile;
		}

		else
		{
			int newBuildableTileID;
			if (GridManager->BlockManager->GetNearestBuildableTileID(newBuildableTileID, lastLinetracedTile, currentBuildToolBlock->GetBlockStats()->Size_X, currentBuildToolBlock->GetBlockStats()->Size_Y, isModuloHalfPi))
			{
				if (newBuildableTileID != -1)
				{
					isBlockClearToBePlaced = true;
					lastBuildableTileID = newBuildableTileID;
				}
			}
		}

		// If the player is resizing a block or if the hovered tile is already used, do not update the block's location to the mouse's position
		if (!isSettingBlockSize && isBlockClearToBePlaced)
		{
			placingBlockTargetLocation = GridManager->GetTileLocation(lastBuildableTileID);
			UE_LOG(LogTemp, Warning, TEXT("Buildable tile is tile ID %d"), lastBuildableTileID);
		}

		// Get tile location to lerp the block to
		currentBuildToolBlock->SetActorLocation(FMath::Lerp(currentBuildToolBlock->GetActorLocation(), placingBlockTargetLocation, blockMovementSpeed * deltaTime));
		currentBuildToolBlock->SetBlockRotation(placingBlockTargetRotation, blockRotationSpeed);
	}
}

void ATT_PlayerGridCamera::ConfirmBuildToolStartTile()
{
	// The tile the block is being stretched from
	placingBlockTileID = lastLinetracedTile;

	isSettingBlockSize = true;
	isMovementEnabled = false;
}


/*---------- Remove Tool / Removing Blocks ----------*/

void ATT_PlayerGridCamera::StartRemoveTool()
{
	StopBuildTool();
	StopRemoveTool();

	isRemoveToolActive = true;
	GetWorldTimerManager().SetTimer(TimerHandle_RemoveTool, this, &ATT_PlayerGridCamera::TickRemoveTool, 0.1f, true, 0.0f);
}

void ATT_PlayerGridCamera::StopRemoveTool()
{
	isRemoveToolSelecting = false;
	isRemoveToolActive = false;
	isMovementEnabled = true;
	GridManager->TileReset(lastLinetracedTile);
	GetWorldTimerManager().ClearTimer(TimerHandle_RemoveTool);
}

void ATT_PlayerGridCamera::TickRemoveTool()
{
	if (isRemoveToolSelecting)
	{
		tilesToBeRemoved = GridManager->BlockManager->GetZoneTileIDsFromZoneParameters(placingBlockTileID, lastLinetracedTile, false);
		GridManager->SetPlayerSelection(tilesToBeRemoved);
		GridManager->SetTileColorToBlockID(tilesToBeRemoved, -1);
		return;
	}
	
	FLinearColor ZoneColour;
	ZoneColour = FVector(0.1, 0.1, 0.1);
	GridManager->SetTileColour(lastLinetracedTile, ZoneColour);
}

void ATT_PlayerGridCamera::ConfirmRemoveToolStartTile()
{
	isRemoveToolSelecting = true;
	isMovementEnabled = false;
	placingBlockTileID = lastLinetracedTile;
}

void ATT_PlayerGridCamera::ConfirmRemoveToolEndTile()
{
	StopRemoveTool();
	GridManager->ClearPlayerSelection();

	for (auto i : tilesToBeRemoved)
	{
		DeleteBlockOnTile(i);
	}
	tilesToBeRemoved.Empty();

}

void ATT_PlayerGridCamera::DeleteBlockOnTile(int tileID)
{
	if (GridManager->IsTileValid(tileID))
	{
		GridManager->BlockManager->DeleteBlockOnTile(tileID);
		GridManager->TileClearState();
		currentLinetracedTile = -1;
	}
}


/*---------- Input binding ----------*/


void ATT_PlayerGridCamera::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MouseY");
	PlayerInputComponent->BindAxis("MouseX");

}

void ATT_PlayerGridCamera::GetTileIDUnderMouse(int32& hoveredTileID, int32& lastHoveredTileID)
{
	hoveredTileID = currentLinetracedTile;
	lastHoveredTileID = lastLinetracedTile;
}
