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

	/*Spawns a grid of tile instances separated by distance in a x by y grid, centered around Center.
	* Calculates all tiles locations & stores them in tileLocations, then spawns the instances using tileSpriteNormal.
	* The tiles will be spawned around a center point.
	* @param x Size X in tiles of the grid.
	* @param y Size Y in tiles of the grid.
	* @param Center Center point world location.
	* @param Distance between each tiles in UE units.
	*/
	void SpawnTiles(int x, int y, FVector center, float distance);

	/*
	 * Spawns a BlockManager object (there can only be one at all times).
	 */
	void SpawnBlockManager();


	/*---------- Variables -----------*/

	/* Size of the grid in tiles on the X axis. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Grid Settings")
		int gridSizeX;

	/* Size of the grid in tiles on the Y axis. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid Settings")
		int gridSizeY;

	/* Distance that separate each tile from one another. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid Settings")
		float distanceBetweenTiles;

	/* Default sprite for a tile. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid Settings")
		UPaperSprite* tileSpriteNormal;

	/* Toggles the spawning of TextRender displaying each tile's ID when they are spawned. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid Settings")
		bool displayTileID;


	/* Array of all Tile IDs in order. */
	TArray<int> tileIDs;
	
	/* Array of all spawned tile instances locations. */
	TArray<FVector> tileLocations; 


	/* TileID of the currently clicked tile. */
	int32 lastClickedTile; 	

	/* Array of all tiles that have been altered. */
	TArray<int32> modifiedTiles; 


	/* Timer responsible of the view mode tile refreshing. */
	FTimerHandle TimerHandler_ViewMode;


	/** Array of all the tiles affected by view modes. */
	TArray<int> viewModeTiles;

	TArray<int> playerTileSelection;

	/* These are the object responsible for displaying the tiles IDs on the grid. */
	TArray<ATextRenderActor*> tileIDActors;
	  

public:	

	/*---------- Components ----------*/
	UPROPERTY(VisibleDefaultsOnly)
		UPaperGroupedSpriteComponent* instanceGroupedSpriteComp;


	/*---------- Blueprint Events -----------*/

	/* Called whenever a tile is hovered by the mouse cursor.
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

	/* Returns the distance between each tile (set via the grid's instanced object). */
	float GetDistanceBetweenTiles();

	/* Returns the size of the grid in a 2D Vector (set via the grid's instanced object). */
	FVector2D GetGridSize();

	/* Returns this tiles location if valid.
	*	@param tileID TileID (instance index) of the tile.
	*	@param WorldSpace Whether or not this should return the world location or relative to the GridManager.
	*	@return Location of the tile
	*/
	UFUNCTION(BlueprintPure, Category = "GridManager")
	FVector GetTileLocation(int tileID, bool WorldSpace);

	/* Returns the tile's neighbours in a clockwise order. If one direction doesn't have a neighbour, nothing will be returned.
	* AllNeighboursTileID returns -1 when a tile doesn't exist, this allows you to use array index to get a specific direction.
	* Direction index: Right 0 - Bottom 1 - Left 2 - Top 3.
	* If allowDiagonalPaths enabled: Right 0 - Bottom/Right 1 - Bottom 2 - Bottom Left 3 etc ...
	* @param tileID Specified tileID.
	* @param allowDiagonalPaths Include the diagonal neighbours
	* @return AllNeighboursTileID This returns all neighbours TileIDs keeping the clockwise order.
	*/
	UFUNCTION(BlueprintPure, Category = "Pathfinder")
	TArray<int> GetTileNeighbours(int tileID, bool allowDiagonalPaths, TArray<int>& AllNeighboursTileID);

	/* Returns the tile's neighbours in a clockwise order. If one direction doesn't have a neighbour, nothing will be returned. 
	* @param tileID Specified tileID.
	* @param allowDiagonalPaths Include the diagonal neighbours.
	*/
	TArray<int> GetTileNeighbours(int tileID, bool allowDiagonalPaths);

	/* Checks if a tileID exists on the grid. */
	UFUNCTION(BlueprintPure, Category = "GridManager")
	bool IsTileValid(int tileID);

	/** Returns an array of all tile IDs in order. */
	UFUNCTION(BlueprintPure, Category = "GridManager")
	TArray<int> GetAllTileIDs();



	/* Reset this tile to its original state (colour and transform). */
	UFUNCTION(BlueprintCallable, Category = "Grid Customisation")
	void TileReset(int tileID);

	/* Set an array of tiles to the color corresponding to the specified block ID. (Color set in data table).
	* @param tileIDs Array of all TileIDs to alter.
	* @param blockID Block ID of the desired colour.
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid Customisation")
	void SetTileColorToBlockID(TArray<int> tileIDs, int blockID);

	/* Set the tile a certain color.
	* @param tileID Tile ID to change color.
	* @param colour Colour to change the tile to.
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid Customisation")
	void SetTileColour(int tileID, FLinearColor colour);

	UFUNCTION(BlueprintCallable, Category = "Grid Customisation")
	void SetTileTransform(int tileID, FTransform newTransform, bool WorldSpace);

	/* Reset all altered tiles to their original state. */
	void TileClearState();


	/*---------- TODO: REFACTOR -----------*/

	/* Save an array of tiles in order to call ClearPlayerSelection() to reset their color/transform. */
	void SetPlayerSelection(TArray<int> selectedTileIDs);

	/* Clears the TArray of tiles, and reset their colour. */
	void ClearPlayerSelection();

	/* Reference to the block manager spawned with SpawnBlockManager(). 
	* TODO: Change all call to this variable to GetBlockManager().
	*/
	ATT_BlockManager* BlockManager;


	/*---------- BlockManager Access -----------*/
	UFUNCTION(BlueprintPure)
	/** Returns a reference to the Blockmanager. */
	ATT_BlockManager* GetBlockManager();

	/**
	 * Returns all the tiles included in the zone delimited by tileA & tileB (opposing corners of the rectangular zone).
	 * @param tileA Tile with the smallest TileID in the zone.
	 * @param tileB Tile with the biggest TileID in the zone.
	 * @param If true, the zone will not include tileB.
	 */
	UFUNCTION(BlueprintPure)
		TArray<int> GetZoneTileIDsFromZoneParameters(int tileA, int tileB, bool excludeTileB);
	
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
