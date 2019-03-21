// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_GridManager.generated.h"

class UPaperGroupedSpriteComponent;
class UPaperSprite;
class UMaterialInterface;
class ATT_BlockManager;

UCLASS()
class TINYTOWN_API ATT_GridManager : public AActor
{
	GENERATED_BODY()
	
public:	

	/*---------- Functions -----------*/
	ATT_GridManager();

	virtual void OnConstruction(const FTransform& Transform) override;


protected:

	/*---------- Components ----------*/

	UPROPERTY()
		USceneComponent* Root;
	

	/*---------- Functions -----------*/
	virtual void BeginPlay() override;

	/** 
	* Spawns a grid of tile instances separated by distance in a x by y grid, centered around Center.
	* Calculates all tiles locations & stores them in tileLocations, then spawns the instances using tileSpriteNormal.
	* The tiles will be spawned around a center point.
	* @param x Size X in tiles of the grid.
	* @param y Size Y in tiles of the grid.
	* @param Center Center point world location.
	* @param Distance between each tiles in UE units.
	*/
	void SpawnTiles(int x, int y, FVector center, float distance); 

	/**
	 * Spawns a BlockManager object (there can only be one at all times).
	 */
	void SpawnBlockManager();

	/* Fetches zone colours from the data table and stores them in an array for use in this object. */
	void FetchZoneColours();

	/*---------- Variables -----------*/

	/** Blueprint class of BlockManager to spawn with SpawnBloackManager(). */
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		TSubclassOf<ATT_BlockManager> BlockManagerClass;

	/** Size of the grid in tiles on the X axis. */
	UPROPERTY(EditAnywhere, Category ="Grid Settings")
		int sizeX;

	/** Size of the grid in tiles on the Y axis. */
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		int sizeY;

	/** Distance that separate each tile from one another. */
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		float distanceBetweenTiles;

	/** Default sprite for a tile. */
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		UPaperSprite* tileSpriteNormal;

	/** Z offset for hovered tiles. */
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		float tileHoveredZOffset;


	/** Array of all spawned tile instances locations. */
	TArray<FVector> tileLocations; 

	/** TileID of the currently clicked tile. */
	int32 clickedTile; 	

	/** Array of all tiles that have been altered. */
	TArray<int32> modifiedTiles; 


	// View modes
	/** Timer responsible of the view mode tile refreshing. */
	FTimerHandle TimerHandler_ViewMode;

	/** Array of all the tiles affected by view modes. */
	TArray<int> viewModeTiles;

	TArray<int> playerTileSelection;

	/* Indicates if any view mode is currently active. */
	bool isViewMode;

	/* Array of all zone colours. */
	TArray<FLinearColor> zoneColours;

	   	 
public:	

	/*---------- Components ----------*/
	UPROPERTY(VisibleDefaultsOnly)
		UPaperGroupedSpriteComponent* instanceGroupedSpriteComp;


	/*---------- Functions -----------*/

	/** Tile Effect - Set this tile as hovered. */
	UFUNCTION(BlueprintCallable)
		void TileHovered(int tileID);

	/** Tile Effect -Set this tile as clicked. */
	UFUNCTION(BlueprintCallable)
		void TileClicked(int tileID);

	/** Tile Effect - Reset this tile to its original state (both colour and transform). */
	void TileReset(int tileID);

	/** Tile Effect - Set this array of tiles as Residential Zone. 
	* @param TileIDs Array of all TileIDs in the zone. 
	* @param ZoneID Block ID of the zone, used to specify a colour. If -1, colour will be Charcoal Grey.
	*/
	UFUNCTION(BlueprintCallable)
		void SetTileColorFromZoneID(TArray<int> tileIDs, int zoneID);

	/** Tile Effect - Set the tile a certain color. */
	void SetTileColor(int tileID, FLinearColor colour);

	/** Tile Effect - Reset all altered tiles to their original state. */
	UFUNCTION(BlueprintCallable)
		void TileClearState();


	/** Accessor - Public accessor for tile locations. 
	*	@param tileID TileID (instance index) of the tile.
	*/
	FVector GetTileLocation(int tileID);

	/** Accessor - Returns the distance between each tile. */
	float GetDistanceBetweenTiles();

	/** Accessor - Returns the size of the grid in a 2D Vector. */
	FVector2D GetGridSize();

	/**
	 *  Activates zone view modes, displays the zone a certain colour on the grid.
	 * Starts a timer to trigger ViewModeTick at a tick like rate.
	 * @param ViewMode Zone's internal index. A zone's int.ID depends on its position in the data table relative to other zones.
	 */
	void ActivateZoneViewMode(int viewMode);

	/** Deactivates all view modes, stops the timer and reset tile colours */
	void StopZoneViewMode();

	/** Depending on the activated view mode, will fetch the zone's tile and apply an effect on them.
	* @TODO: Avoid accessing database on tick,  ~3ms when view mode active.
	*/
	void ViewModeTick();

	void SetPlayerSelection(TArray<int> tileIDs);
	void ClearPlayerSelection();


	/*---------- Variables -----------*/
 
	/** Reference to the block manager spawned with SpawnBlockManager(). */
	ATT_BlockManager* BlockManager; 
	   	 
};
