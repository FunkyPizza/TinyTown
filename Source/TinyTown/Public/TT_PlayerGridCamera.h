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
class UTT_Pathfinder;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UTT_Pathfinder* PathfinderComp;


/*---------- Functions -----------*/

	virtual void BeginPlay() override;

	/** Handles camera movements via mouse/touch. */
	void MouseMovementsTick();

	/** Handles camera rotation via mouse. */
	void MouseRotationTick(); 

	/** Handles zoom via mouse. Zooms in or out step by step, steps being defined in BeginPlay.
	* @param value If <0 will zoom out, if >0 will zoom in.
	*/
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	void InputCameraZoom(float value); 

	/* Handles the left click input for things such as selecting a block, placing one down or simply interacting with the grid. */
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	void InputSelectButtonDown(); 

	/* Handles the left click input for things such as confirming the remove tool or interacting with the grid. */
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	void InputSelectButtonUp();

	/* Start rotating the camera using input axis "MouseX" and "MouseY". */
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	void InputRotationButtonDown(); 

	/* Stop rotating the camera using input axis "MouseX" and "MouseY". */
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	void InputRotationButtonUp();

	/* Start moving the camera using input axis "MouseX" and "MouseY". */
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	void InputMoveButtonDown();

	/* Stop moving the camera using input axis "MouseX" and "MouseY". */
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	void InputMoveButtonUp();
	
	/* Cancels the building of a block and deactivates the remove tool. */
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	void InputCancel();

	/* Select a block from one of its tile IDs. If the tile isn't owned by a block, nothing will happen. */
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	bool SelectBlockFromTileID(int tileID);

	/* Select a block from a reference to the block itself. If the input block isn't valid, nothing will happen. */
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	bool SelectBlock(ATT_Block* blockToSelect);

	/* Deselects currently selected block. */
	UFUNCTION(BlueprintCallable, Category = "Player Input")
	void DeselectBlock();

	/* Line trace from the camera to the grid and updates tile if they are hovered. */
	void MouseTrace(); 

	/* Moves the camera in XY direction multiplied by Sensitivity. XY are axis values (-1 < value < 1).
	* @param x Direction X to move in (-1 < X < 1).
	* @param y Direction Y to move in (-1 < x < 1).
	* @param sensitivity Used to scale movement speed (defined by blueprint variable).
	*/
	UFUNCTION(BlueprintCallable)
	void MoveCamera(float x, float y, float sensitivity); 

	/* Rotates the camera around Y and Z axis multiplied by Sensitivity.  XY are axis values (-1 < value < 1).
	* @param x Amount of Yaw to add.
	* @param y Amount of Pitch to add.
	* @param xSensitivity Used to scale Yaw rotation speed (defined by blueprint variable).
	* @param ySensitivity Used to scale Pitch rotation speed (defined by blueprint variable).
	*/
	UFUNCTION(BlueprintCallable)
	void RotateCamera(float x, float y, float xSensitivity, float ySensitivity);

	/* Returns a reference to the grid manager this object is using. */
	UFUNCTION(BlueprintPure)
	ATT_GridManager* GetGridManager(); 

	/*Gets a reference to the blockmanager from the gridmanager. */
	UFUNCTION(BlueprintPure)
	ATT_BlockManager* GetBlockManager();

	/* Spawns a block in edit mode (with rotation and movements enables) to show the player where he is placing down the block.
	 * Block placement is confirmed by calling ConfirmBuildTool() or cancelled with StopBuildTool(). 
	 * @param blockID Data table index of the row corresponding to the block to spawn.
	 */ 
	UFUNCTION(BlueprintCallable, Category = "Block Building")
	void StartBuildTool(int blockID);

	/* Cancels the building of a block. */
	UFUNCTION(BlueprintCallable, Category = "Block Building")
	void StopBuildTool();

	/* Lerps the "ghost block" on tick to ensure a smooth movement. If placing a zone, this will update the grid to show what tiles are being affected.
	 * @param deltaTime deltaTime value being passed through via the Tick function.
	 */
	void TickBuildTool(float deltaTime);

	/* If placing a Zone or Path, use this to confirm the first tile of the zone or path. 
	* This allows the user  to hold click and drag to place down this type of block.
	 */
	void ConfirmBuildToolStartTile();

	/* Confirms the placement of the block, and call the logic to spawn the block on the grid.
	*/
	UFUNCTION(BlueprintCallable, Category = "Block Building")
	void ConfirmBuildTool();


	/* Start the  removeTool. The player will have to click and drag to select a zone of tiles to clear. */
	UFUNCTION(BlueprintCallable, Category = "Block Building")
	void StartRemoveTool();

	/* Stop the remove tool and cancel the current selection. */
	UFUNCTION(BlueprintCallable, Category = "Block Building")
	void StopRemoveTool();

	/* Used to refresh the zone selection. */
	void TickRemoveTool();

	/* This confirms the currently hovered tile to be the first tile of the RemoveTool selection zone. */
	void ConfirmRemoveToolStartTile();

	/* To be called when StartTile has been confirmed. This concludes the RemoveTool, deleting selected tile and disabling the tool. */
	void ConfirmRemoveToolEndTile();

	/* Calls DeleteBlockOnTile in BlockManager to delete the block or clear the zone a TileID.
	 * @param tileID TileID of the tile to clear/ owned by the block to delete.
	 */
	UFUNCTION(BlueprintCallable, Category = "Block Building")
	void DeleteBlockOnTile(int tileID);

	
	
	/*---------- Variables -----------*/

	/** Movement sensitivity for moving the camera with mouse input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Settings")
	float mouseMovementSpeed = 175.0f;

	/** Y axis rotation sensitivity using mouse input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Settings")
	float mouseYRotationSpeed = 3.0f;

	/** X axis rotation sensitivity using mouse input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Settings")
		float mouseXRotationSpeed = 4.0f;

	/** Movement sensitivity for moving the camera with keyboard. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Settings")
	float keyboardMovementSpeed = 20.0f;

	/** Movement sensitivity for rotating the camera with keyboard. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Settings")
	float keyboardRotationSpeed = 1.5f;

	/** Maximum pitch in degrees the camera is allowed to rotate to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float maximumPitch = 85.0f;

	/**	Minimum pitch in degrees the camera is allowed to rotate to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
		float minimumPitch = 5.0f;

	/** Maximum length for the spring arm. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float MaxSpringArmLength = 12000.0f;

	/** Minimum length for the spring arm. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	float MinSpringArmLength = 1500.0f;

	/** Defines how accurately the player can zoom (the higher, the slower the zoom but more accurate). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	int zoomSteps = 10;

	/** Defines how fast the camera lerps when zooming and de-zooming (the higher, the faster). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Settings")
	int zoomLerpSpeed = 5;

	/** The speed at which a ghost block moves on the grid (when placing down a block). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Building")
	float blockMovementSpeed = 12.0f;

	/** The speed at which a ghost block moves on the grid (when placing down a block). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Building")
		float blockRotationSpeed = 0.01f;

	/** How fast can the mouse rotate the ghostBlock when placing down a block. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Building")
	float blockRotationMouseThreshold = 0.1f;


	FTimerHandle TimerHandle_MouseMovements;
	FTimerHandle TimerHandle_MouseRotation;

	// Block building variables

	/* This is a reference to the block currently being placed down. */
	UPROPERTY(BlueprintReadOnly, Category = "Block Building")
	ATT_Block* currentBuildToolBlock; 

	/* This is a reference to the currently selected block.*/
	UPROPERTY(BlueprintReadOnly, Category = "Block Building")
		ATT_Block* selectedBlock;

	int placingBlockID; // BlockID of the ghostBlock that was spawned
	int placingBlockTileID; // Tile on which the block has been spawned (used as StartTile for zone spawning)
	int placingBlocklastEndTileID; // Value of EndTileID last time zone was calculated
	bool isPlacingDownAResizableBlock; // Indicates whether the ghostBlock that was spawn is resizable OR can be rotated
	TArray<int> placingLastZoneBuilt; // The tile array of the last zone to be placed down
	FVector placingBlockTargetLocation; // Target location to lerp to (when placing a building and hovering tiles)
	FRotator placingBlockTargetRotation = FRotator(0,0,0); // Target rotation to lerp to (when rotating the ghostBlock)
	float placingBlockMouseX; // X Mouse position at beginning of ghostBlock rotation
	float placingBlockMouseY; // Y Mouse position at beginning of ghostBlock rotation
	int lastBuildableTileID;

	// Remove tool
	TArray<int> tilesToBeRemoved;
	FTimerHandle TimerHandle_RemoveTool;

	/** Reference to the current GridManager, set by GetGridManager(). */
	ATT_GridManager* GridManager; 


	//Zoom variables
	int zoomCurrentStep; // Current zoom step (this * zoomCoefficient = ArmLength) 
	int zoomCoefficient; // Distance between two zoomSteps
	float newTargetArmLength; // Target ArmLength to lerp to


	// Status variables
	UPROPERTY(BlueprintReadWrite, Category = "Status")
	bool isMovementEnabled; // Dictates whether the player can move the camera or not
	UPROPERTY(BlueprintReadOnly, Category = "Status")
	bool isSelectButtonDown; // Indicates if the Select Button (Left click / touch) is down
	UPROPERTY(BlueprintReadOnly, Category = "Status")
	bool isRotationButtonDown; // Indicates if the Rotate button (middle mouse / 2 fingers) is down
	UPROPERTY(BlueprintReadOnly, Category = "Status")
	bool isMoveButtonDown; // Indicates if the Move button (right click) is down
	UPROPERTY(BlueprintReadOnly, Category = "Status")
	bool isMovingCamera; // Indicates if the camera is being moved
	UPROPERTY(BlueprintReadOnly, Category = "Status")
	bool isRotatingCamera; // Indicates if the camera is being rotated
	UPROPERTY(BlueprintReadOnly, Category = "Status")
	bool isPlacingDownABlock; // Indicates if the player is placing down a building
	UPROPERTY(BlueprintReadOnly, Category = "Status")
	bool isBlockSelected;

	bool isRotatingBlock; // Indicates if the player is rotating a building while placing it down
	bool isSettingBlockSize; // Indicates if the player is setting the size of a block while placing it down (drag)
	bool isPlacingDownAPath; // Indicates whether or not the block is a road in which case pathfinding should be use
	bool isZoneBuildingCancelled; // Indicates whether block building should be restarted or totally cancelled.

	UPROPERTY(BlueprintReadOnly, Category = "Status")
	bool isRemoveToolActive; // Indicates whether the RemoveTool is activated.
	bool isRemoveToolSelecting; // Indicates whether the RemoveTool currently has selected tiles to remove or not.
	int32 currentLinetracedTile; // Tile ID of the current line traced tile, if none returns -1
	int32 lastLinetracedTile; // Tile ID of the last line traced tile
	int lastPathGoalTile; // Goal tile ID of the last path that was found


public:	

	/*---------- Functions -----------*/

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Is called every time a second passes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TimeSystem")
		void OnBlockSelected();

	/** Is called every time a second passes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TimeSystem")
		void OnBlockDeselected();

	/**
	* Returns the tile currently hovered tile. Will return -1 if no tiles is hovered.
	* @output hoveredTileID		TileID of the tile under the cursor.
	* @output lastHoveredTileID TileId of the last tile to be hovered.
	*/
	UFUNCTION(BlueprintPure)
	void GetTileIDUnderMouse(int32& hoveredTileID, int32& lastHoveredTileID);
	
};
