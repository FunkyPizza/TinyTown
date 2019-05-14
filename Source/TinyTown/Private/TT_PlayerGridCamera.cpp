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

/*---------- Primary functions ----------*/
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

	// Enables mouse input to move the camera
	InputCameraMovements();
	InputCameraRotation();

}

void ATT_PlayerGridCamera::BeginPlay()
{
	Super::BeginPlay();

	//Get the grid manager the player interacts with 
	GridManager = GetGridManager();

	//Enables camera movement
	isMovementEnabled = true;

	//Set camera zoom values
	zoomCurrentStep = zoomStepAccuracy / 2;
	zoomCoefficient = (MaxSpringArmLength - MinSpringArmLength) / zoomStepAccuracy;
	InputCameraZoom(1);
}

ATT_GridManager* ATT_PlayerGridCamera::GetGridManager()
{
	// Iterate through actors of class TT_GridManager and gets first result. (There should be only one at all times)
	for (TActorIterator<ATT_GridManager> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		if (ActorItr->IsA(ATT_GridManager::StaticClass()))
		{
			return *ActorItr;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Couldn't get GridManager in PlayerGridCamera."))
	return nullptr;
}


/*---------- Player input functions ----------*/

// Keyboard inputs
void ATT_PlayerGridCamera::InputKeyboardMovements(float notused)
{
	if (isMovementEnabled) 
	{
		isMovingCamera = true;

		MoveCamera(GetInputAxisValue("MoveRight"), GetInputAxisValue("MoveForward"), inputMovementKeyboardSensitivity);
	}

	else 
	{
		isMovingCamera = false;
	}
}

void ATT_PlayerGridCamera::InputKeyboardRotation(float notused)
{
	if (isMovementEnabled) 
	{
		isRotatingCamera = true;

		RotateCamera(-notused, 0.0f, inputRotationKeyboardSensitivty, 0);
	}

	else 
	{
		isRotatingCamera = false;
	}
}

// Mouse inputs
void ATT_PlayerGridCamera::InputCameraMovements()
{
	if (isMovementEnabled) 
	{
		if (isMoveButtonDown) 
		{
			isMovingCamera = true;

			MoveCamera(-GetInputAxisValue("MouseX"), -GetInputAxisValue("MouseY"), inputMovementMouseSensitivity);
		}

		else 
		{
			isMovingCamera = false;
		}
	}
}

void ATT_PlayerGridCamera::InputCameraRotation()
{
	if (isMovementEnabled) 
	{
		if (isRotationButtonDown) 
		{
			isRotatingCamera = true;

			RotateCamera(GetInputAxisValue("MouseX"), GetInputAxisValue("MouseY"), inputXRotationSensitivity, inputYRotationSensitivity);
		}

		else 
		{
			isRotatingCamera = false;
		}
	}
}

void ATT_PlayerGridCamera::InputCameraZoom(float value)
{
	if (isMovementEnabled && value != 0) 
	{
		// Zoom out
		if (value < -0.1f) {
			zoomCurrentStep = FMath::Clamp(zoomCurrentStep + 1, 1, zoomStepAccuracy);

			newTargetArmLength = zoomCurrentStep * zoomCoefficient + MinSpringArmLength;
		}

		// Zoom in
		if (value > 0.1f) 
		{
			zoomCurrentStep = FMath::Clamp(zoomCurrentStep - 1, 1, zoomStepAccuracy);

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
		GridManager->TileClicked(currentLinetracedTile);
	}
}

void ATT_PlayerGridCamera::InputSelectButtonUp()
{
	isSelectButtonDown = false;

	if (currentLinetracedTile != -1)
	{
		GridManager->TileClearState();
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

void ATT_PlayerGridCamera::InputRotationButtonDown()
{
	isRotationButtonDown = true;
}

void ATT_PlayerGridCamera::InputRotationButtonUp()
{
	isRotationButtonDown = false;
}

void ATT_PlayerGridCamera::InputMoveButtonDown()
{
	isMoveButtonDown = true;
}

void ATT_PlayerGridCamera::InputMoveButtonUp()
{
	isMoveButtonDown = false;
}

void ATT_PlayerGridCamera::InputCancel()
{
	if (isPlacingDownABlock)
	{
		isZoneBuildingCancelled = true;
		StopBuildTool();
		return;
	}

	if (isRemoveToolActive)
	{
		StopRemoveTool();
		return;
	}
}


/*---------- Camera movement functions ----------*/

void ATT_PlayerGridCamera::MoveCamera(float x, float y, float sensitivity)
{
	//Avoids a speed increase when both X and Y axis = 1.
	float axisYNormalized = y * FMath::Abs(FMath::Sin(FMath::Atan2(y, x)));
	float axisXNormalized = x * FMath::Abs(FMath::Cos(FMath::Atan2(y, x)));

	FVector movementVector = FVector(axisYNormalized * sensitivity, axisXNormalized * sensitivity, 0.0f);

	AddActorLocalOffset(movementVector);
}

void ATT_PlayerGridCamera::RotateCamera(float x, float y, float xSensitivity, float ySensitivity)
{

	FRotator currentRotation = SpringArmComp->GetRelativeTransform().GetRotation().Rotator();

	// Clamps camera angle 
	if (currentRotation.Pitch >= -5.0f )
	{
		SpringArmComp->SetRelativeRotation(FRotator(-5.1f, 0, 0));
		return;
	}
	if (currentRotation.Pitch <= -85.0f) 
	{
		SpringArmComp->SetRelativeRotation(FRotator(-84.9f, 0, 0));
		return;
	}

	// Rotate Y
	FRotator newRotationY = FRotator(y, 0, 0) * ySensitivity;
	SpringArmComp->AddLocalRotation(newRotationY);

	// Rotate X
	FRotator newRotationX = FRotator(0, x, 0) * xSensitivity;
	AddActorLocalRotation(newRotationX);
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
				if (Hit.Actor == GridManager && Hit.Item != currentLinetracedTile)
				{
					// Hit a tile
					currentLinetracedTile = Hit.Item;
					lastLinetracedTile = currentLinetracedTile;

					if (!isSettingBlockSize)
					{
						GridManager->TileHovered(currentLinetracedTile);
					}
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

	if (placingBlockInstance)
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
	placingBlockInstance = GetWorld()->SpawnActorDeferred<ATT_Block>(placingBlockStats->BlockClass, blockTransform);

	if (placingBlockInstance)
	{
		placingBlockInstance->SetBlockStats(placingBlockStats);
		placingBlockInstance->SetBlockManager(GridManager->BlockManager);
		placingBlockInstance->ActivateEditMode();

		UGameplayStatics::FinishSpawningActor(placingBlockInstance, blockTransform);

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

		placingBlockInstance->Destroy();
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
	if (!placingBlockInstance)
	{
		placingBlockID = -1;
		UE_LOG(LogTemp, Warning, TEXT("The placing block instance is not valid, cannot spawn block"));
		return;
	}

	// Block has a fixed size
	if (!isPlacingDownAResizableBlock)
	{
		GridManager->BlockManager->SpawnBlock(placingBlockID, placingBlockTargetRotation, lastLinetracedTile);
	}

	// Block is a zone / is resizable
	if (isPlacingDownAResizableBlock)
	{
		GridManager->BlockManager->CreateZoneOnTiles(placingLastZoneBuilt, placingBlockID);
		GridManager->ClearPlayerSelection();

		for (auto i : placingLastZoneBuilt)
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
	if (placingBlockInstance)
	{
		if (isPlacingDownAResizableBlock)
		{
			TArray<int> tempTiles;
			tempTiles.Add(lastLinetracedTile);
			GridManager->SetTileColorFromZoneID(tempTiles, placingBlockID);
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

					placingLastZoneBuilt = PathfinderComp->FindShortestPathInZoneDijkstra(placingBlockTileID, lastLinetracedTile, GetZoneTileIDsFromZoneParameters(placingBlockTileID, lastLinetracedTile), false, placingBlockID);

					if (placingLastZoneBuilt.Num() < 5)
					{
						placingLastZoneBuilt = PathfinderComp->FindShortestPathAStar(placingBlockTileID, lastLinetracedTile, false, placingBlockID);
					}

					lastPathGoalTile = lastLinetracedTile;
				}
					GridManager->SetPlayerSelection(placingLastZoneBuilt);
					GridManager->SetTileColorFromZoneID(placingLastZoneBuilt, placingBlockID);
			}

			else 
			{
				placingLastZoneBuilt = GetZoneTileIDsFromZoneParameters(placingBlockTileID, lastLinetracedTile);
				GridManager->SetPlayerSelection(placingLastZoneBuilt);
				GridManager->SetTileColorFromZoneID(placingLastZoneBuilt, placingBlockID);

			}
		}

		// If not, rotate it
		else if (!isZoneBuildingCancelled)
		{
			// Check that the mouse button is held and that the block has finished its previous rotation
			if (isSelectButtonDown && FMath::IsNearlyEqual(placingBlockInstance->RotationRoot->RelativeRotation.Yaw, placingBlockTargetRotation.Yaw, 0.1f))
			{
				// Check for any mouse movement meaning the player wants to rotate the block
				if (!(FMath::IsNearlyEqual(GetInputAxisValue("MouseX"), 0.0f, ghostBlockRotationMouseThreshold)))
				{
					isRotatingBlock = true;
					isMovementEnabled = false;
				}

				if (GetInputAxisValue("MouseX") > ghostBlockRotationMouseThreshold)
				{
					placingBlockTargetRotation = placingBlockTargetRotation - FRotator(0, 90, 0);

					if (placingBlockTargetRotation.Yaw < -180)
					{
						placingBlockTargetRotation = FRotator(0, 90, 0);
					}
				}

				if (GetInputAxisValue("MouseX") < -ghostBlockRotationMouseThreshold)
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
		if (!isSettingBlockSize)
		{
			bool isModuloHalfPi = FMath::IsNearlyEqual(abs(placingBlockTargetRotation.Yaw), 90, 0.1f);
			int tileA = GridManager->BlockManager->GetZoneStartTileFromHoveredTile(lastLinetracedTile, placingBlockInstance->GetBlockStats()->Size_X, placingBlockInstance->GetBlockStats()->Size_Y, isModuloHalfPi);
			int tileB = GridManager->BlockManager->GetZoneEndTileFromZoneSize(tileA, placingBlockInstance->GetBlockStats()->Size_X, placingBlockInstance->GetBlockStats()->Size_Y, isModuloHalfPi);
			TArray<int> ghostBlockTiles = GridManager->BlockManager->GetZoneTileIDsFromZoneParameters(tileA, tileB);

			
			for (auto i : ghostBlockTiles)
			{
				if (GridManager->BlockManager->GetSpawnedBlockIDs()[i] != 0)
				{
					isBlockClearToBePlaced = false;
				}
			}
		}

		// If the player is resizing a block or if the hovered tile is already used, do not update the block's location to the mouse's position
		if (!isSettingBlockSize && isBlockClearToBePlaced)
		{
			placingBlockTargetLocation = GridManager->GetTileLocation(lastLinetracedTile);
		}

		// Get tile location to lerp the block to
		placingBlockInstance->SetActorLocation(FMath::Lerp(placingBlockInstance->GetActorLocation(), placingBlockTargetLocation, ghostBlockMovementSpeed * deltaTime));
		placingBlockInstance->SetBlockRotation(placingBlockTargetRotation);
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
		tilesToBeRemoved = GetZoneTileIDsFromZoneParameters(placingBlockTileID, lastLinetracedTile);
		GridManager->SetPlayerSelection(tilesToBeRemoved);
		GridManager->SetTileColorFromZoneID(tilesToBeRemoved, -1);
		return;
	}
	
	FLinearColor ZoneColour;
	ZoneColour = FVector(0.1, 0.1, 0.1);
	GridManager->SetTileColor(lastLinetracedTile, ZoneColour);
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

void ATT_PlayerGridCamera::RemoveBlockUnderCursor()
{
	DeleteBlockOnTile(lastLinetracedTile);
}

void ATT_PlayerGridCamera::DeleteBlockOnTile(int tileID)
{
	GridManager->BlockManager->DeleteBlockOnTile(tileID);
	GridManager->TileClearState();
	currentLinetracedTile = -1;
}


/*---------- Other functions ----------*/

TArray<int> ATT_PlayerGridCamera::GetZoneTileIDsFromZoneParameters(int tileA, int tileB)
{
	TArray<int> TileIDs;

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


	for (int i = 0; i <= abs(blockSize.Y); i++)
	{
		for (int j = 0; j <= abs(blockSize.X); j++)
		{
			int newTileID = tileA + j * xSign + (i * ySign  * GridManager->GetGridSize().X);

			TileIDs.Add(newTileID);
		}
	}

	return TileIDs;
}

void ATT_PlayerGridCamera::ToggleViewMode(int ViewMode)
{
	GridManager->ActivateZoneViewMode(ViewMode);
}


/*---------- Input binding ----------*/

void ATT_PlayerGridCamera::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Keyboard
	PlayerInputComponent->BindAxis("MoveForward", this, &ATT_PlayerGridCamera::InputKeyboardMovements);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATT_PlayerGridCamera::InputKeyboardMovements);
	PlayerInputComponent->BindAxis("XRotation", this, &ATT_PlayerGridCamera::InputKeyboardRotation);

	//PROTOTYPE Build input
	PlayerInputComponent->BindAction("Delete", IE_Pressed, this, &ATT_PlayerGridCamera::RemoveBlockUnderCursor);

	//Mouse
	PlayerInputComponent->BindAction("InputSelect", IE_Pressed, this, &ATT_PlayerGridCamera::InputSelectButtonDown);
	PlayerInputComponent->BindAction("InputSelect", IE_Released, this, &ATT_PlayerGridCamera::InputSelectButtonUp);
	PlayerInputComponent->BindAction("InputCancel", IE_Released, this, &ATT_PlayerGridCamera::InputCancel);
	PlayerInputComponent->BindAction("InputRotation", IE_Pressed, this, &ATT_PlayerGridCamera::InputRotationButtonDown);
	PlayerInputComponent->BindAction("InputRotation", IE_Released, this, &ATT_PlayerGridCamera::InputRotationButtonUp);
	PlayerInputComponent->BindAction("InputMovement", IE_Pressed, this, &ATT_PlayerGridCamera::InputMoveButtonDown);
	PlayerInputComponent->BindAction("InputMovement", IE_Released, this, &ATT_PlayerGridCamera::InputMoveButtonUp);
	PlayerInputComponent->BindAxis("ZoomIn", this, &ATT_PlayerGridCamera::InputCameraZoom);
	PlayerInputComponent->BindAxis("MouseY");
	PlayerInputComponent->BindAxis("MouseX");

}
