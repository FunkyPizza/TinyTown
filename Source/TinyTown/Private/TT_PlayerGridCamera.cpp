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

/*---------- Primary functions ----------*/
ATT_PlayerGridCamera::ATT_PlayerGridCamera()
{	
/*---------- Components creation ----------*/
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	
/*---------- Setting defaults ----------*/
	PrimaryActorTick.bCanEverTick = true;

	lastLinetracedTile = 0;

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

void ATT_PlayerGridCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update SpringArmLength with a lerp
	if (SpringArmComp->TargetArmLength != newTargetArmLength)
	{
		SpringArmComp->TargetArmLength = FMath::Lerp(SpringArmComp->TargetArmLength, newTargetArmLength, zoomLerpSpeed * DeltaTime);
	}

	// Update placingBlockGhost location to follow mouse
	if (isPlacingDownBlock)
	{
		TickBuilding(DeltaTime);
	}

	// Linetrace cursor to 3D world
	MouseTrace();

	// Enables mouse input to move the camera
	InputCameraMovements();
	InputCameraRotation();


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

void ATT_PlayerGridCamera::InputKeyboardMovements(float donotuse)
{
	if (isMovementEnabled) 
	{
		isMovingCamera = true;

		MoveCamera(GetInputAxisValue("MoveRight"), GetInputAxisValue("MoveForward"), cameraDragKeyboardSensitivty);
	}

	else 
	{
		isMovingCamera = false;
	}
}

void ATT_PlayerGridCamera::InputKeyboardRotation(float value)
{
	if (isMovementEnabled) 
	{
		isRotatingCamera = true;

		RotateCamera(-value, 0.0f, cameraRotationKeyboardSensitivty, 0);
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

			MoveCamera(-GetInputAxisValue("MouseX"), -GetInputAxisValue("MouseY"), cameraDragSensitivty);
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

			RotateCamera(GetInputAxisValue("MouseX"), GetInputAxisValue("MouseY"), cameraRotationXSensitivity, cameraRotationYSensitivity);
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

	if (isPlacingDownBlock)
	{
		// Temporarily save mouse position (used to set mouse position back after building rotation)
		GetWorld()->GetFirstPlayerController()->GetMousePosition(placingBlockMouseX, placingBlockMouseY);

		// Activate zone building if player is placing down a resizable block
		if (isGhostBlockResizable && !isSettingBlockSize)
		{
			ActivateZoneBuilding();
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

	// Set mouse position back to where it was before block rotation and stop block rotation.
	if (isRotatingBlock)
	{
		GetWorld()->GetFirstPlayerController()->SetMouseLocation(placingBlockMouseX, placingBlockMouseY);
		isRotatingBlock = false;
		return;
	}

	// If not rotating block, confirm block building locaiton.
	if (isPlacingDownBlock && !isSettingBlockSize)
	{
		FinishBuilding();
		return;
	}
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



/*---------- Camera movement functions ----------*/
void ATT_PlayerGridCamera::MoveCamera(float X, float Y, float sensitivity)
{
	//Avoids a speed increase when both X and Y axis = 1.
	float axisYNormalized = Y * FMath::Abs(FMath::Sin(FMath::Atan2(Y, X)));
	float axisXNormalized = X * FMath::Abs(FMath::Cos(FMath::Atan2(Y, X)));

	FVector movementVector = FVector(axisYNormalized * sensitivity, axisXNormalized * sensitivity, 0.0f);

	AddActorLocalOffset(movementVector);
}

void ATT_PlayerGridCamera::RotateCamera(float X, float Y, float XSensit, float YSensit)
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
	FRotator newRotationY = FRotator(Y, 0, 0) * YSensit;
	SpringArmComp->AddLocalRotation(newRotationY);

	// Rotate X
	FRotator newRotationX = FRotator(0, X, 0) * XSensit;
	AddActorLocalRotation(newRotationX);
}


/*---------- Grid interaction functions ----------*/
void ATT_PlayerGridCamera::MouseTrace()
{
	// Checks if there is a valid TT_GridManager and only line traces if the player hasn't clicked yet
	if (GridManager)
	{
		if (!isSelectButtonDown || isSettingBlockSize)
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


// Block building
void ATT_PlayerGridCamera::StartBuilding(int BlockID)
{
	if (!placingBlockGhostClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Class placingBlockGhost has not been set in %s."), *GetName());
		return;
	}

	if (placingBlockGhost)
	{
		// Cancel previous block building
		CancelBuiding();
	}

	// Ghost block setting
	placingBlockGhostID = BlockID;
	isGhostBlockResizable = GridManager->BlockManager->GetBlockStatsFromBlockID(placingBlockGhostID)->Resizable;
	FTransform blockTransform = FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(1,1,1));
	placingBlockTargetLocation = FVector(0, 0, 0);
	placingBlockTargetRotation = FRotator(0, 0, 0);
	   
	if (isGhostBlockResizable)
	{
		placingBlockGhost = GetWorld()->SpawnActorDeferred<ATT_Block>(placingResizableBlockGhostClass, blockTransform);
	}
	else
	{
		placingBlockGhost = GetWorld()->SpawnActorDeferred<ATT_Block>(placingBlockGhostClass, blockTransform);
	}

	if (placingBlockGhost)
	{
		placingBlockGhost->SetBlockStats(GridManager->BlockManager->GetBlockStatsFromBlockID(placingBlockGhostID));
		placingBlockGhost->SetBlockManager(GridManager->BlockManager);

		UGameplayStatics::FinishSpawningActor(placingBlockGhost, blockTransform);


		isPlacingDownBlock = true;
	}
}

void ATT_PlayerGridCamera::FinishBuilding()
{
	if (placingBlockGhostID == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("placingBlockGhostID was not set, not spawning block"));
		return;
	}

	if (!placingBlockGhost)
	{
		placingBlockGhostID = -1;
		UE_LOG(LogTemp, Warning, TEXT("placingBlockGhost was not created, not spawning block"));
		return;
	}

	// Building block
	if (!isGhostBlockResizable)
	{
		BuildBlockOnTile(lastLinetracedTile, placingBlockGhostID, placingBlockTargetRotation);
	}
	if (isGhostBlockResizable)
	{

	}

	CancelBuiding();

}

void ATT_PlayerGridCamera::CancelBuiding()
{
	if (isPlacingDownBlock)
	{
		isPlacingDownBlock = false;
		isSettingBlockSize = false;
		isGhostBlockResizable = false;
		GridManager->TileClearState();

		placingBlockGhost->Destroy();
		placingBlockGhostID = -1;

		isMovementEnabled = true;
	}
}

void ATT_PlayerGridCamera::TickBuilding(float deltaTime)
{
	if (placingBlockGhost)
	{
		// Enables keyboard movement when placing down a block
		if (!isSelectButtonDown)
		{
			isMovementEnabled = true;
		}

		// Resizing the block
		if (isSettingBlockSize)
		{
			placingLastZoneBuilt = CalculateZoneTileIDs(placingBlockTileID, lastLinetracedTile);
			GridManager->TileZoneRes(placingLastZoneBuilt);
		}

		// If not, rotate it
		else
		{
			// Check that the mouse button is held and that the block has finished its previous rotation
			if (isSelectButtonDown && FMath::IsNearlyEqual(placingBlockGhost->GetActorRotation().Yaw, placingBlockTargetRotation.Yaw, 0.1f))
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

		// If the player is resizing a block, do not update the block's location to the mouse's position
		if (!isSettingBlockSize)
		{
			placingBlockTargetLocation = GridManager->GetTileLocation(lastLinetracedTile) - FVector(0, GridManager->GetDistanceBetweenTiles(), 0);
		}

		// Get tile location to lerp the block to
		placingBlockGhost->SetActorLocation(FMath::Lerp(placingBlockGhost->GetActorLocation(), placingBlockTargetLocation, ghostBlockMovementSpeed * deltaTime));
		placingBlockGhost->SetActorRotation(FMath::Lerp(placingBlockGhost->GetActorRotation(), placingBlockTargetRotation, ghostBlockRotationSpeed * deltaTime));
	}
}

void ATT_PlayerGridCamera::BuildBlockOnTile(int TileID, int BlockID, FRotator BlockRotation)
{
	GridManager->BlockManager->CreateBlockOnTile(TileID, BlockID, BlockRotation);
}

// Resizable block building
void ATT_PlayerGridCamera::ActivateZoneBuilding()
{
	// The tile the block is being stretched from
	placingBlockTileID = lastLinetracedTile;

	isSettingBlockSize = true;
	isMovementEnabled = false;
}

TArray<int> ATT_PlayerGridCamera::CalculateZoneTileIDs(int StartTile, int EndTile)
{
	TArray<int> TileIDs;
	UE_LOG(LogTemp, Log, TEXT("CalculatingZoneTileIDs in PlayerGridCamera."));

	if (placingBlocklastEndTileID != EndTile && StartTile != 0 && EndTile != 0)
	{
		placingBlocklastEndTileID = EndTile;

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


		for (int i = 0; i <= abs(blockSize.Y); i++)
		{
			for (int j = 0; j <= abs(blockSize.X); j++)
			{
				int newTileID = StartTile + j * xSign + (i * ySign  * GridManager->GetGridSize().X);

				TileIDs.Add(newTileID);
			}
		}
		return TileIDs;
	}
	
	return TileIDs;
}

// Block Actions 
void ATT_PlayerGridCamera::DeleteBlockOnTile(int TileID)
{
	GridManager->BlockManager->DeleteBlockOnTile(TileID);
}


// PROTOTYPE Build function
void ATT_PlayerGridCamera::Build0()
{
	StartBuilding(1);
}
void ATT_PlayerGridCamera::Build1()
{
	StartBuilding(2);
}
void ATT_PlayerGridCamera::Build2()
{
	StartBuilding(3);
}
void ATT_PlayerGridCamera::Build3()
{
	StartBuilding(7);
}
void ATT_PlayerGridCamera::DestroyBlockUnderCursor()
{
	DeleteBlockOnTile(lastLinetracedTile);
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
	PlayerInputComponent->BindAction("0", IE_Pressed, this, &ATT_PlayerGridCamera::Build0);
	PlayerInputComponent->BindAction("1", IE_Pressed, this, &ATT_PlayerGridCamera::Build1);
	PlayerInputComponent->BindAction("2", IE_Pressed, this, &ATT_PlayerGridCamera::Build2);
	PlayerInputComponent->BindAction("3", IE_Pressed, this, &ATT_PlayerGridCamera::Build3);
	PlayerInputComponent->BindAction("Delete", IE_Pressed, this, &ATT_PlayerGridCamera::DestroyBlockUnderCursor);


	//Mouse
	PlayerInputComponent->BindAction("InputSelect", IE_Pressed, this, &ATT_PlayerGridCamera::InputSelectButtonDown);
	PlayerInputComponent->BindAction("InputSelect", IE_Released, this, &ATT_PlayerGridCamera::InputSelectButtonUp);
	PlayerInputComponent->BindAction("InputCancel", IE_Pressed, this, &ATT_PlayerGridCamera::CancelBuiding);
	PlayerInputComponent->BindAction("InputCancel", IE_Released, this, &ATT_PlayerGridCamera::CancelBuiding);
	PlayerInputComponent->BindAction("InputRotation", IE_Pressed, this, &ATT_PlayerGridCamera::InputRotationButtonDown);
	PlayerInputComponent->BindAction("InputRotation", IE_Released, this, &ATT_PlayerGridCamera::InputRotationButtonUp);
	PlayerInputComponent->BindAction("InputMovement", IE_Pressed, this, &ATT_PlayerGridCamera::InputMoveButtonDown);
	PlayerInputComponent->BindAction("InputMovement", IE_Released, this, &ATT_PlayerGridCamera::InputMoveButtonUp);
	PlayerInputComponent->BindAxis("ZoomIn", this, &ATT_PlayerGridCamera::InputCameraZoom);
	PlayerInputComponent->BindAxis("MouseY");
	PlayerInputComponent->BindAxis("MouseX");

}
