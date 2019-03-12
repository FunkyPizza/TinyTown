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

	/** Handles camera movements via keyboard
	* @params notused This parameter enables the function to be bound to an AxisInput. It is not used.
	*/
	void InputKeyboardMovements(float notused);

	/** Handles camera rotation via keyboard.
* @params notused This parameter enables the function to be bound to an AxisInput. It is not used.
*/
	void InputKeyboardRotation(float notused);

	/** Handles camera movements via mouse/touch. */
	void InputCameraMovements();

	/** Handles camera rotation via mouse. */
	void InputCameraRotation(); 

	/** Handles zoom via mouse. Zooms in or out step by step, steps being defined in BeginPlay.
	* @params value If <0 will zoom out, else if >0 will zoom in.
	*/
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

	/** Linetrace from the camera to the grid and updates tile if they are hovered. */
	void MouseTrace(); 

	/** Moves the camera in XY direction multiplied by Sensitivity.
	* @params x Direction X to move in (-1 < X < 1).
	* @params y Direction Y to move in (-1 < x < 1).
	* @params sensitivity Used to scale movement speed (defined by blueprint variable).
	*/
	void MoveCamera(float x, float y, float sensitivity); 

	/** Rotates the camera around Y and Z axis multiplied by Sensitivity
	* @params x Amount of Yaw to add.
	* @params y Amount of Pitch to add.
	* @params xSensitivity Used to scale Yaw rotation speed (defined by blueprint variable).
	* @params ySensitivity Used to scale Pitch rotation speed (defined by blueprint variable).
	*/
	void RotateCamera(float x, float y, float xSensitivity, float ySensitivity);

	/** 
	* Finds and saves the GridManager as a reference.
	* This uses the first existing GridManager object that it finds, doesn't support multiple GridManager objects.
	*/
	ATT_GridManager* GetGridManager(); 

	/**
	 * Spawns a block in edit mode (aka "ghostblock") to show the player where he is placing down the block.
	 * @params blockID Data table index of the row corresponding to the block to spawn.
	 */ 
	UFUNCTION(BlueprintCallable)
	void StartBuilding(int blockID);

	/**
	 * If the building is flagged as "Resizable", the player will instead be placing down a zone instead
	 * of a block. This will avoid the building being placed down on first click and instead will require a drag & drop.
	 */
	void ActivateZoneBuilding();

	/**
	* Finalises the block building or zone building process. Calls the BlockManager to build the block and ensures
	* the temporary "ghostblock" is destroyed.
	*/
	void FinishBuilding();

	/**
	 * Cancels the building of block if any. Ensures the temporary "ghostblock" is destroyed.
	 */
	void CancelBuiding();

	/**
	 * Lerps the "ghostblock" on tick to ensure a smooth movement. If placing a zone, this will
	 * update the grid to show what tiles are being affected.
	 * @params deltaTime deltaTime value being passed through via the Tick function.
	 */
	void TickBuilding(float deltaTime);

	/**
	 * Calls DeleteBlockOnTile in BlockManager to delete the block or clear the zone a TileID.
	 * @params tileID TileID of the tile to clear/ owned by the block to delete.
	 */ 
	void DeleteBlockOnTile(int tileID);

	/**
	 * Returns all the tiles included in the zone delimited by tileA & tileB (opposing corners of the rectangular zone).
	 * This function is very similar to the one in BlockManager to the exception of the for loop (<= instead of <).
	 * @params tileA Corner A / StartTile of the zone.
	 * @params tileB Opposite corner to A.
	 */
	TArray<int> GetZoneTileIDsFromZoneParameters(int tileA, int tileB);

	/**
	 * Calls ActivateZoneViewMode in GridManager.
	 * @params ViewMode ViewMode ID (should be replaced by EBuildingType)
	 */
	UFUNCTION(BlueprintCallable)
		void ToggleViewMode(int ViewMode);


	//PROTOTYPE Build function
	void DestroyBlockUnderCursor();


/*---------- Variables -----------*/

	/** Movement sensitivity for moving the camera with mouse input. */
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float inputMovementMouseSensitivity;

	/** Movement sensitivity for moving the camera with keyboard. */
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float inputMovementKeyboardSensitivity;

	/** Y axis rotation sensitivity using mouse input. */
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float inputYRotationSensitivity;

	/** X axis rotation sensitivity using mouse input. */ 
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float inputXRotationSensitivity;

	/** Movement sensitivity for rotating the camera with keyboard. */
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float inputRotationKeyboardSensitivty;

	/** Maximum length for the spring arm. */
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MaxSpringArmLength;

	/** Minimum length for the spring arm. */
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MinSpringArmLength;

	/** Defines how accurately the player can zoom (the higher, the slower the zoom). */
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	int zoomStepAccuracy;

	/** Defines how fast the camera lerps when zooming and de-zooming (the higher, the faster). */
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	int zoomLerpSpeed;

	/** The class responsible to display blocks being placed down.. */
	UPROPERTY(EditAnywhere, Category = "Block Building")
	TSubclassOf<ATT_Block> placingBlockGhostClass;

	/** The class responsible to display resizable block being placed down. */
	UPROPERTY(EditAnywhere, Category = "Block Building")
	TSubclassOf<ATT_Block> placingResizableBlockGhostClass;

	/** The speed at which a ghost block moves on the grid (when placing down a block). */
	UPROPERTY(EditAnywhere, Category = "Block Building")
	float ghostBlockMovementSpeed;

	/** How fast can the mouse rotate the ghostBlock when placing down a block. */
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


	/** Reference to the current GridManager, set by GetGridManager(). */
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
