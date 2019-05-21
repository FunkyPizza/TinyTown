// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_GridManager.generated.h"

class UPaperGroupedSpriteComponent;
class UPaperSprite;
class UMaterialInterface;
class ATT_BlockManager;
class ATextRenderActor;


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

	/** Size of the grid in tiles on the X axis. */
	UPROPERTY(EditAnywhere, Category ="Grid Settings")
		int gridSizeX;

	/** Size of the grid in tiles on the Y axis. */
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		int gridSizeY;

	/** Distance that separate each tile from one another. */
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		float distanceBetweenTiles;

	/** Default sprite for a tile. */
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		UPaperSprite* tileSpriteNormal;

	/** Toggles the spawning of TextRender displaying each tile's ID when they are spawned. */
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		bool displayTileID;


	/** Array of all Tile IDs in order. */
	TArray<int> tileIDs;
	
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

	TArray<ATextRenderActor*> tileIDActors;
	   	 
public:	

	/*---------- Components ----------*/
	UPROPERTY(VisibleDefaultsOnly)
		UPaperGroupedSpriteComponent* instanceGroupedSpriteComp;


	/*---------- Events -----------*/

	/** 
	* Called whenever a tile is hovered by the mouse cursor.
	* @param tileID TileID of the tile that has been hovered.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Grid Customisation")
		void OnTileHovered(int tileID);

	/** Tile Effect - Set this tile as hovered. */
	void OnTileHovered_Implementation(int tileID);

	/**
	* Called whenever a tile is clicked by the mouse cursor.
	* @param tileID TileID of the tile that has been clicked.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Grid Customisation")
		void OnTileClicked(int tileID);

	/** Tile Effect -Set this tile as clicked. */
	void OnTileClicked_Implementation(int tileID);

	/*---------- Functions -----------*/

	/** Tile Effect - Reset this tile to its original state (both colour and transform). */
	void TileReset(int tileID);

	/** Tile Effect - Set this array of tiles as Residential Zone. 
	* @param TileIDs Array of all TileIDs in the zone. 
	* @param ZoneID Block ID of the zone, used to specify a colour. If -1, colour will be Charcoal Grey.
	*/
	void SetTileColorFromZoneID(TArray<int> zoneTileIDs, int zoneID);

	/** Tile Effect - Set the tile a certain color. */
	UFUNCTION(BlueprintCallable, Category = "Grid Customisation")
	void SetTileColor(int tileID, FLinearColor colour);

	/** Tile Effect - Reset all altered tiles to their original state. */
		void TileClearState();


	/** Accessor - Public accessor for tile locations. 
	*	@param tileID TileID (instance index) of the tile.
	*/
	UFUNCTION(BlueprintPure, Category = "GridManager")
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

	/** Depending on the activated view mode, will fetch the zone's tile and apply an effect on them. */
	void ViewModeTick();

	/** Set a TArray of tiles' colour to avoid being overridden by view modes. */
	void SetPlayerSelection(TArray<int> selectedTileIDs);

	/** Clears the TArray of tiles, and reset their colour. */
	void ClearPlayerSelection();

	/** Checks if a tileID exists on the grid. */
	UFUNCTION(BlueprintPure, Category = "GridManager")
	bool IsTileValid(int tileID);

	/** Returns an array of all tile IDs in order. */
	UFUNCTION(BlueprintPure, Category = "GridManager")
	TArray<int> GetAllTileIDs();


	/*---------- Variables -----------*/
 
	/** Reference to the block manager spawned with SpawnBlockManager(). */
	ATT_BlockManager* BlockManager; 
	   	 

	/*---------- BlockManager Access -----------*/
	UFUNCTION(BlueprintPure)
	/** Returns a reference to the Blockmanager. */
	ATT_BlockManager* GetBlockManager();

	/**
	 * Returns all the tiles included in the zone delimited by tileA & tileB (opposing corners of the rectangular zone).
	 * @param tileA Tile with the smallest TileID in the zone.
	 * @param tileB Tile with the biggest TileID in the zone.
	 */
	UFUNCTION(BlueprintPure)
		TArray<int> GetZoneTileIDsFromZoneParameters(int tileA, int tileB);
	
	/**
 * Returns the tileID of Tile A (corner with the smallest TileID in the zone) associated with the zone defined by parameters.
 * @param tileC Tile at the center of the zone (in some cases this is the tile hovered by the mouse).
 * @param sizeX X size in tile of the zone
 * @param sizeY Y size in tile of the zone
 */
	UFUNCTION(BlueprintPure)
		int GetZoneTileAFromHoveredTile(int tileC, int sizeX, int sizeY);

	/**
 * Returns the TileID of the corner tile opposite to tileA in a zone defined by parameters (see top of page for zone explanation).
 * @param tileA Tile with the smallest TileID in the zone. 
 * @param sizeX X size in tile of the zone
 * @param sizeY Y size in tile of the zone
 */
	UFUNCTION(BlueprintPure)
		int GetZoneTileBFromZoneSize(int tileA, int sizeX, int sizeY);

	/**
* Returns the x and y size of a zone from the array of its TileIDs.
* @param zone Array containing all TileIDs of the zone.
*/
	UFUNCTION(BlueprintPure)
		FVector2D GetZoneSizeFromTileArray(TArray<int> zone);

};
