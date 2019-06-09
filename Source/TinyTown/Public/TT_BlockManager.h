// Fill out your copyright notice in the Description page of Project Settings.

	/* Blocks bigger than 1x1 are characterised by a StartTile (A), a EndTile(B) and the zone's size for X and Y.
			_____________																_____________
			|_2_|_5_|_8_|	Given this grid, and a block of size X=2, Y=3 where			|_x_|_x_|_x_|
			|_1_|_4_|_7_|	A=1 has a zone represented by the "x" tiles. In		 -->	|_x_|_x_|_x_|
			|_0_|_3_|_6_|	case B=8.													|___|___|___|

	/!\ Remember, X represent the vertical axis, Y the horizontal when it comes to tile calculation /!\ */


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_Global.h"
#include "TT_BlockManager.generated.h"

class ATT_GridManager;
class UDataTable;
class ATT_Block;
struct FTT_Struct_Block;
struct FTT_Struct_BlockType;

UCLASS()
class TINYTOWN_API ATT_BlockManager : public AActor
{
	GENERATED_BODY()
	
public:	

	ATT_BlockManager();


protected:
	/*---------- Functions -----------*/

	virtual void BeginPlay() override;
	
	/**
	* Returns the data table located at static path (see definition).	
	*/
	UDataTable* GetBlockDataTable();

	/**
	* This function sorts the data base into a FMap. It separates all different block types and gather all blockIDs from the 
	* same type in the same place.
	*/
	void RefreshDataFromDataTable();

	/**
	 * Resize all the tile arrays (responsible of holding tile information, such as if it is used, what is it used by etc ...)
	 * to the number of tiles there are in the level.
	 * @param NumberOfTiles Number to resize the arrays to.
	 */
	void SetTileArraysSize(int newArraySize);

	/**
	 * Clear any value in tile arrays at the specified index.
	 * @param index Index of the tile to clear (index = instance index of the tile)
	 */
	void ClearTileArraysAtIndex(int index);


	void FindZoneLayout(int zoneID, TArray<int> zone);


	/*---------- Variables -----------*/



	/** Data table holding data of all the blocks.*/
	UDataTable* data_Block;

	/** This map sorts all blocks by types, each key is a type (string) containing an array of BlockID (int). */
	TMap<FString, FTT_Struct_BlockType> blockTypeMap;
	
public:	

	/*---------- Functions -----------*/

	/** Sets the GridManager variable and sets the size of all tile arrays.
	* @param newGridManager New value of GridManager
	*/
	void SetGridManager(ATT_GridManager* newGridManager);

	/**
	* Calculate the zone used by the block & assign the tile arrays to the block.
	* Spawn & initialize the block (passes through data table data, tile belonging to the block etc ..)
	* @param blockID Data table index of the row corresponding to the block to spawn.
	* @param blockRotation Orientation of the block. (Can only be % Pi/2 (0°, 90°, 180°)).
	 * @param tileID Index of the tile to spawn the block around.
	 */
	void SpawnBlock(int blockID, FRotator blockRotation, int tileID);

	/**
	* Calculate the zone used by the block & assign the tile arrays to the block.
	* Spawn & initialize the block (passes through data table data, tile belonging to the block etc ..).
	* The block will be spawn from its zone's StartTile rather than around the tile. Doesn't support block rotation.
	* @param blockID Data table index of the row corresponding to the block to spawn.
	* @param tileID Index of the tile to be set as the block's zone's StartTile.
 */
	void SpawnBlockAtStartTile(int blockID, int tileID);

	/**
	* Gets a random blockID corresponding to parameters in the data table.
	* Calls SpawnBlock to finalise the spawning.
	* @param tileID Data table index of the row corresponding to the block.
	* @param blockRotation Orientation of the block. (Can only be % Pi/2 (0°, 90°, 180°)).
	* @param buildingType Type of building to look for (should be replaced by EBuildingType).
	* @param efficiency Level of the building to look for (1-3)
	* @param sizeX X size of block's zone (how big is the block in tiles)
	* @param sizeY Y size of block's zone (how big is the block in tiles)
	*/
	void SpawnBlockFromParameters(int tileID, FRotator blockRotation, FString buildingType, int efficiency, int sizeX, int sizeY);

	/**
	* Delete block that owns the tile selected & clears all tiles that belonged to that block. Calls DestroyBlock in the block.
	* If the tile has no block, calls DeleteZoneOnTile.
	* @param tileID TileID of the tile to clear/ owned by the block to delete.
	*/
	void DeleteBlockOnTile(int tileID);

	/**
* Assigns elements of the spawnedZoneID array to a certain ZoneID.
* @param tileIDs TileIDs of the zone's tiles.
* @param zoneID ID of the zone to assign the tiles to.
*/
	void CreateZoneOnTiles(TArray<int> tileIDs, int zoneID);

	/**
	* Clears the tile of any zoneIDs.
	* @param tileID TileID of the tile to clear of a zone.
	*/
	void DeleteZoneOnTile(int tileID);

	/**
	 * Returns all the tiles included in the zone delimited by tileA & tileB (opposing corners of the rectangular zone).
	 * (see top of page for zone explanation)
	 * @param tileA Corner A / StartTile of the zone.
	 * @param tileB Opposite corner to A.
	 */
	TArray<int> GetZoneTileIDsFromZoneParameters(int tileA, int tileB);

	/**
	 * Returns the TileID of the StartTile associated with the zone defined by parameters (see top of page for zone explanation).
	 * @param tileC Tile currently hovered by the mouse.
	 * @param sizeX X size of block's zone (how big is the block in tiles).
	 * @param sizeY Y size of block's zone (how big is the block in tiles).
	 * @param isModuloHalfPi If true, sizeX = sizeY & sizeY = sizeX (depending on the block's orientation).
	 */
	int GetZoneStartTileFromHoveredTile(int tileC, int sizeX, int sizeY, bool isModuloHalfPi);

	/**
	* Returns the tileID of the tile that would have been hovered if this block the standard way.
	* @
	*/
	int GetHoveredTileFromZoneParameter(int tileC, int sizeX, int sizeY, bool isModuloHalfPi);
	/**
	 * Returns the TileID of the corner tile opposite to tileA in a zone defined by parameters (see top of page for zone explanation).
	 * Caution, this functions doesn't take in account the edges of the grid.
	 * @param tileA Corner A / StartTile of the zone.
	 * @param sizeX X size of block's zone (how big is the block in tiles).
	 * @param sizeY Y size of block's zone (how big is the block in tiles).
	 * @param isModuloHalfPi If true, sizeX = sizeY & sizeY = sizeX (depending on the block's orientation).
	 */
	int GetZoneEndTileFromZoneSize(int tileA, int sizeX, int sizeY, bool isModuloHalfPi);
	/**
* Returns the x and y size of a zone from the array of its TileIDs.
* @param zone Array containing all TileIDs of the zone.
*/
	FVector2D GetZoneSizeFromTileArray(TArray<int> zone);

	/**
	* Returns pointer to data of block from its BlockID (see TT_Struct_Block).
	* @param blockID Data table index of the row corresponding to the block to spawn.
	*/
	FTT_Struct_Block* GetBlockStatsFromBlockID(int blockID);

	/** 
	* Returns data of block from its BlockID (see TT_Struct_Block). Added to expose GetBlockStatsFromBlockID() to blueprints.
	* @param blockID Data table index of the row corresponding to the block to spawn.
	*/
	UFUNCTION(BlueprintPure, Category = "BlockManager")
	const FTT_Struct_Block GetBlockStatsInDataTable(int blockID);

	/**
	* Returns a random blockID corresponding to parameters in the data table.
	* @param buildingType Type of building to look for (see EBuildingType).
	* @param efficiency Level of the building to look for (1-3)
	* @param sizeX X size of block's zone (how big is the block in tiles)
	* @param sizeY Y size of block's zone (how big is the block in tiles)
	*/
	int GetRandomBlockIDFromParameter(FString buildingType, int efficiency, int sizeX, int sizeY);

	/**
	* Returns an array of all the blockID corresponding to parameters in the data table.
	* @param buildingType Type of building to look for (see EBuildingType).
	* @param efficiency Level of the building to look for (1-3)
	* @param sizeX X size of block's zone (how big is the block in tiles)
	* @param sizeY Y size of block's zone (how big is the block in tiles)
	*/
	TArray<int> GetAllBlockIDsFromParameter(FString buildingType, int efficiency, int sizeX, int sizeY);

	/* Accessor - Returns the array of spawned zones where index = index of the tile, and element = BlockID. */
	UFUNCTION(BlueprintPure, Category = "BlockManager")
	TArray<int> GetSpawnedZoneTileIDs();

	/* Accessor - Returns the array of spawned blocks where index = index of the tile, and element = BlockID. */
	UFUNCTION(BlueprintPure, Category = "BlockManager")
	TArray<int> GetSpawnedBlockIDs();

	/* Accessor - Returns the array of spawned blocks where index = index of the tile, and element = reference to the ATT_Block object. */
	UFUNCTION(BlueprintPure, Category = "BlockManager")
	TArray<ATT_Block*> GetSpawnedBlocks();

	

	/*---------- Variables -----------*/
	
		/** Tile Array - Array of spawned block IDs where index = index of the tile, and element = BlockID.*/
	TArray<int> spawnedBlockID;

	/** Tile Array - Array of spawned zone IDs where index = index of the tile, and element = BlockID. */
	TArray<int> spawnedZoneID;

	/** Tile Array - Array of spawned blocks where index = index of the tile, and element = TT_Block instance.*/
	TArray<ATT_Block*> spawnedBlocks;

	/** Reference to the GridManager who created this block manager.*/
	ATT_GridManager* GridManager;

	/* This maps contains contains all the blocks flagged as BT_Zone block names alongside their blockID. */
	TMap<FString, int> zoneIDMap;

	/* This maps contains contains all the blocks fallaged BT_ZoneBuilding block names alongside their blockID. */
	TMap<FString, int> zoneBuildingIDMap;

	/* This array of bool indicates which zone view mode is activate. For each block of type "zone", there is a bool. */
	TArray<bool> isZoneViewModeActive;

	/* This is array is mapped like isZoneViewModeActive. It holds the original BlockID of each zone. */
	TArray<int> zoneViewModeIndex;


	/*---------- Blueprints Exposed -----------*/

	/** This returns a map of all the blocks from the datatable organised by Categories (set by the user in the table). 
	* Each key is a category and contains all the block IDs associated with its category and their names. This includes zones and zone buildings.
	*/
	UFUNCTION(BlueprintPure, Category = "BlockManager")
	TMap<FString, FTT_Struct_BlockType> GetAllBlocksSortedByCategory();

	/** This returns a map of all the blocks flagged as Zones.
	* Each key is the name of a zone and contains the zone's block ID.
	*/
	UFUNCTION(BlueprintPure, Category = "BlockManager")
		TMap<FString, int>  GetAllZones();

	/** This returns a map of all the blocks flagged as Zone buildings.
	* Each key is the name of the zone the building is associated to, and contains the block zone building's block ID.
	*/
	UFUNCTION(BlueprintPure, Category = "BlockManager")
		TMap<FString, int>  GetAllZoneBuildings();

	
};
