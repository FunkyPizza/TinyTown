// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TT_PlayerGridCamera.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class ATT_GridManager;
class ATT_Block;

UCLASS()
class TINYTOWN_API ATT_PlayerGridCamera : public APawn
{
	GENERATED_BODY()
	
public:	

/*---------- Functions -----------*/
	ATT_PlayerGridCamera();


protected:

/*---------- Components ----------*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* CameraComp;


/*---------- Functions -----------*/

	virtual void BeginPlay() override;

	// Handles camera movements via keyboard
	void InputKeyboardMovements(float donotuse); 

	// Handles camera movements via mouse/touch
	void InputCameraMovements();

	//Handles camera rotation via mouse
	void InputCameraRotation(); 

	//Handles camera rotation via keyboard
	void InputKeyboardRotation(float value); 

	// Handles zoom via mouse
	void InputCameraZoom(float value); 

	//Handles Select button inputs
	void InputSelectButtonDown(); 
	void InputSelectButtonUp();

	//Handles Camera Rotation button inputs
	void InputRotationButtonDown(); 
	void InputRotationButtonUp();

	//Handles Camera Movement button inputs
	void InputMoveButtonDown();
	void InputMoveButtonUp();

	// Get hit results under cursor, handles tile detection
	void MouseTrace(); 

	// Moves the camera in XY direction multiplied by Sensitivity
	void MoveCamera(float X, float Y, float sensitivity); 

	// Rotates the camera around Y and Z axis multiplied by Sensitivity
	void RotateCamera(float X, float Y, float XSensit, float YSensit); 

	// TActor iterator to find grid manager
	ATT_GridManager* GetGridManager(); 

	// Spawns a ghostBlock that follows the mouse to indicate where the player is building
	UFUNCTION(BlueprintCallable)
	void StartBuilding(int BlockID);

	void ActivateZoneBuilding();

	// Confirms the building location and rotation
	void FinishBuilding();

	// Cancels the building of a block
	void CancelBuiding();

	// Enables the movement and rotation of the ghostBlock
	void TickBuilding(float deltaTime);

	// Calls BuildBlockOnTile in BlockManager
	void BuildBlockOnTile(int TileID, int BlockID, FRotator BlockRotation);

	// Calls DeleteBlockOnTile in BlockManager
	void DeleteBlockOnTile(int TileID);

	// Gets all TileIDs contained in the rectangular zone (where params are opposite corners)
	TArray<int> CalculateZoneTileIDs(int StartTile, int EndTile);

	//PROTOTYPE Build function
	void Build0();
	void Build1();
	void Build2();
	void Build3();
	void DestroyBlockUnderCursor();
 

/*---------- Variables -----------*/

	// Movement sensitivity for moving the camera with mouse or touch drag
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float cameraDragSensitivty;

	// Movement sensitivity for moving the camera with keyboard
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float cameraDragKeyboardSensitivty;

	// Y axis rotation sensitivity using mouse or touch 
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float cameraRotationYSensitivity;

	// X axis rotation sensitivity using mouse or touch 
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float cameraRotationXSensitivity;

	// Movement sensitivity for moving the camera with keyboard
	UPROPERTY(EditAnywhere, Category = "Input Settings")
		float cameraRotationKeyboardSensitivty;

	// Maximum length for the spring arm
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MaxSpringArmLength;

	// Minimum length for the spring arm
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MinSpringArmLength;

	// Defines how accurately the player can zoom (the higher, the slower the zoom)
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	int zoomStepAccuracy;

	// Defines how fast the camera lerps when zooming and de-zooming (the higher, the faster)
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	int zoomLerpSpeed;

	// The class responsible to display blocks being placed down.
	UPROPERTY(EditAnywhere, Category = "Block Building")
	TSubclassOf<ATT_Block> placingBlockGhostClass;

	// The class responsible to display resizable block being placed down.
	UPROPERTY(EditAnywhere, Category = "Block Building")
		TSubclassOf<ATT_Block> placingResizableBlockGhostClass;

	// The speed at which a ghost block moves on the grid (when placing down a block).
	UPROPERTY(EditAnywhere, Category = "Block Building")
	float ghostBlockMovementSpeed;

	// How fast can the mouse rotate the ghostBlock when placing down a block.
	UPROPERTY(EditAnywhere, Category = "Block Building")
	float ghostBlockRotationMouseThreshold;


	//Block building variables
	ATT_Block* placingBlockGhost; // Currently spawned ghostBlock
	int placingBlockGhostID; // BlockID of the ghostBlock that was spawned
	int placingBlockTileID; // Tile on which the block has been spawned (used as StartTile for zone spawning)
	int placingBlocklastEndTileID; // Value of EndTileID last time zone was calculated
	bool isGhostBlockResizable; // Indicates whether the ghostBlock that was spawn is resizable OR can be rotated
	TArray<int> placingLastZoneBuilt; // The tile array of the last zone to be placed down
	FVector placingBlockTargetLocation; // Target location to lerp to (when placing a building and hovering tiles)
	FRotator placingBlockTargetRotation; // Target rotation to lerp to (when rotating the ghostBlock)
	float placingBlockMouseX; // X Mouse position at beginning of ghostBlock rotation
	float placingBlockMouseY; // Y Mouse position at beginning of ghostBlock rotation


	// Current grid manager
	ATT_GridManager* GridManager; 


	//Zoom variables
	int zoomCurrentStep; // Current zoom step (this * zoomCoefficient = ArmLength) 
	int zoomCoefficient; // Distance between two zoomSteps
	float newTargetArmLength; // Target ArmLength to lerp to


	// Status variables
	bool isMovementEnabled; // Dictates whether the player can move the camera or not
	bool isSelectButtonDown; // Indicates if the Select Button (Left click / touch) is down
	bool isRotationButtonDown; // Indicates if the Rotate button (middle mouse / 2 fingers) is down
	bool isMoveButtonDown; // Indicates if the Move button (right click) is down
	bool isMovingCamera; // Indicates if the camera is being moved
	bool isRotatingCamera; // Indicates if the camera is being rotated
	bool isPlacingDownBlock; // Indicates if the player is placing down a building
	bool isRotatingBlock; // Indicates if the player is rotating a building while placing it down
	bool isSettingBlockSize; // Indicates if the player is setting the size of a block while placing it down (drag)
	int32 currentLinetracedTile; // Tile ID of the current line traced tile, if none returns -1
	int32 lastLinetracedTile; // Tile ID of the last line traced tile


public:	

	/*---------- Functions -----------*/
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
