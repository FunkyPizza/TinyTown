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

	/** Handles camera movements via keyboard
	* @param notused This parameter enables the function to be bound to an AxisInput. It is not used.
	*/
	void InputKeyboardMovements(float notused);

	/** Handles camera rotation via keyboard.
* @param notused This parameter enables the function to be bound to an AxisInput. It is not used.
*/
	void InputKeyboardRotation(float notused);

	/** Handles camera movements via mouse/touch. */
	void InputCameraMovements();

	/** Handles camera rotation via mouse. */
	void InputCameraRotation(); 

	/** Handles zoom via mouse. Zooms in or out step by step, steps being defined in BeginPlay.
	* @param value If <0 will zoom out, else if >0 will zoom in.
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
	
	//Handles input to cancel a building
	void InputCancel();

	/** Line trace from the camera to the grid and updates tile if they are hovered. */
	void MouseTrace(); 

	/** Moves the camera in XY direction multiplied by Sensitivity.
	* @param x Direction X to move in (-1 < X < 1).
	* @param y Direction Y to move in (-1 < x < 1).
	* @param sensitivity Used to scale movement speed (defined by blueprint variable).
	*/
	void MoveCamera(float x, float y, float sensitivity); 

	/** Rotates the camera around Y and Z axis multiplied by Sensitivity
	* @param x Amount of Yaw to add.
	* @param y Amount of Pitch to add.
	* @param xSensitivity Used to scale Yaw rotation speed (defined by blueprint variable).
	* @param ySensitivity Used to scale Pitch rotation speed (defined by blueprint variable).
	*/
	void RotateCamera(float x, float y, float xSensitivity, float ySensitivity);

	/** 
	* Finds and saves the GridManager as a reference.
	* This uses the first existing GridManager object that it finds, doesn't support multiple GridManager objects.
	*/
	UFUNCTION(BlueprintPure)
	ATT_GridManager* GetGridManager(); 

	/**
	* Gets a reference to the blockmanager from the gridmanager.
	*/
	UFUNCTION(BlueprintPure)
	ATT_BlockManager* GetBlockManager();

	/**
	 * Spawns a block in edit mode (aka "ghost block") to show the player where he is placing down the block.
	 * @param blockID Data table index of the row corresponding to the block to spawn.
	 */ 
	UFUNCTION(BlueprintCallable)
	void StartBuildTool(int blockID);

	/** Cancels the building of block if any. Ensures the temporary "ghost block" is destroyed. */
	void StopBuildTool();

	/** Lerps the "ghost block" on tick to ensure a smooth movement. If placing a zone, this will update the grid to show what tiles are being affected.
	 * @param deltaTime deltaTime value being passed through via the Tick function.
	 */
	void TickBuildTool(float deltaTime);

	/** If the building is flagged as "Resizable", the player will instead be placing down a zone instead
	 * of a block. This will avoid the building being placed down on first click and instead will require a drag & drop.
	 */
	void ConfirmBuildToolStartTile();

	/** Finalises the block building or zone building process. Calls the BlockManager to build the block and ensures the temporary "ghost block" is destroyed. */
	void ConfirmBuildTool();


	/** Starts RemoveTool, will wait for the player to click a tile to call ConfirmRemoveToolStartTile(). */
	UFUNCTION(BlueprintCallable)
	void StartRemoveTool();

	/** Fully Disables the RemoveTool, and cancels any tile deletion. */
	void StopRemoveTool();

	/** Used to refresh the zone selection. */
	void TickRemoveTool();

	/** This confirms the currently hovered tile to be the first tile of the RemoveTool selection zone. */
	void ConfirmRemoveToolStartTile();

	/** To be called when StartTile has been confirmed. This concludes the RemoveTool, deleting selected tile and disabling the tool. */
	void ConfirmRemoveToolEndTile();

	/** Instantly deletes whatever is placed on the currently hovered tile. */
	void RemoveBlockUnderCursor();

	/** Calls DeleteBlockOnTile in BlockManager to delete the block or clear the zone a TileID.
	 * @param tileID TileID of the tile to clear/ owned by the block to delete.
	 */
	void DeleteBlockOnTile(int tileID);


	/** Calls ActivateZoneViewMode in GridManager.
	 * @param ViewMode ViewMode ID (should be replaced by EBuildingType)
	 */
	UFUNCTION(BlueprintCallable)
	void ToggleViewMode(int ViewMode);

	/** Returns all the tiles included in the zone delimited by tileA & tileB (opposing corners of the rectangular zone).
	 * This function is very similar to the one in BlockManager to the exception of the for loop (<= instead of <).
	 * @param tileA Corner A / StartTile of the zone.
	 * @param tileB Opposite corner to A.
	 */
	TArray<int> GetZoneTileIDsFromZoneParameters(int tileA, int tileB);
	
	
	/*---------- Variables -----------*/

	/** Movement sensitivity for moving the camera with mouse input. */
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float mouseMovementSpeed;

	/** Y axis rotation sensitivity using mouse input. */
	UPROPERTY(EditAnywhere, Category = "Input Settings")
		float mouseYRotationSpeed;

	/** X axis rotation sensitivity using mouse input. */
	UPROPERTY(EditAnywhere, Category = "Input Settings")
		float mouseXRotationSpeed;

	/** Movement sensitivity for moving the camera with keyboard. */
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float keyboardMovementSpeed;

	/** Movement sensitivity for rotating the camera with keyboard. */
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float keyboardRotationSpeed;

	/** Maximum length for the spring arm. */
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MaxSpringArmLength;

	/** Minimum length for the spring arm. */
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MinSpringArmLength;

	/** Defines how accurately the player can zoom (the higher, the slower the zoom but more accurate). */
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	int zoomSteps;

	/** Defines how fast the camera lerps when zooming and de-zooming (the higher, the faster). */
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	int zoomLerpSpeed;

	/** The speed at which a ghost block moves on the grid (when placing down a block). */
	UPROPERTY(EditAnywhere, Category = "Block Building")
	float blockMovementSpeed;

	/** The speed at which a ghost block moves on the grid (when placing down a block). */
	UPROPERTY(EditAnywhere, Category = "Block Building")
		float blockRotationSpeed;

	/** How fast can the mouse rotate the ghostBlock when placing down a block. */
	UPROPERTY(EditAnywhere, Category = "Block Building")
	float blockRotationMouseThreshold;


	// Block building variables
	ATT_Block* placingBlockInstance; // Currently spawned ghostBlock
	int placingBlockID; // BlockID of the ghostBlock that was spawned
	int placingBlockTileID; // Tile on which the block has been spawned (used as StartTile for zone spawning)
	int placingBlocklastEndTileID; // Value of EndTileID last time zone was calculated
	bool isPlacingDownAResizableBlock; // Indicates whether the ghostBlock that was spawn is resizable OR can be rotated
	TArray<int> placingLastZoneBuilt; // The tile array of the last zone to be placed down
	FVector placingBlockTargetLocation; // Target location to lerp to (when placing a building and hovering tiles)
	FRotator placingBlockTargetRotation; // Target rotation to lerp to (when rotating the ghostBlock)
	float placingBlockMouseX; // X Mouse position at beginning of ghostBlock rotation
	float placingBlockMouseY; // Y Mouse position at beginning of ghostBlock rotation

	// Remove tool
	TArray<int> tilesToBeRemoved;
	FTimerHandle TimerHandle_RemoveTool;

	/** Reference to the current GridManager, set by GetGridManager(). */
	UPROPERTY(BlueprintReadOnly)
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
	bool isPlacingDownABlock; // Indicates if the player is placing down a building
	bool isRotatingBlock; // Indicates if the player is rotating a building while placing it down
	bool isSettingBlockSize; // Indicates if the player is setting the size of a block while placing it down (drag)
	bool isPlacingDownAPath; // Indicates whether or not the block is a road in which case pathfinding should be use
	bool isZoneBuildingCancelled; // Indicates whether block building should be restarted or totally cancelled.
	bool isRemoveToolActive; // Indicates whether the RemoveTool is activated.
	bool isRemoveToolSelecting; // Indicates whether the RemoveTool currently has selected tiles to remove or not.
	int32 currentLinetracedTile; // Tile ID of the current line traced tile, if none returns -1
	int32 lastLinetracedTile; // Tile ID of the last line traced tile
	int lastPathGoalTile; // Goal tile ID of the last path that was found


public:	

	/*---------- Functions -----------*/

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	* Returns the tile currently hovered tile. Will return -1 if no tiles is hovered.
	* @output hoveredTileID		TileID of the tile under the cursor.
	* @output lastHoveredTileID TileId of the last tile to be hovered.
	*/
	UFUNCTION(BlueprintPure)
	void GetTileIDUnderMouse(int32& hoveredTileID, int32& lastHoveredTileID);
	
};
