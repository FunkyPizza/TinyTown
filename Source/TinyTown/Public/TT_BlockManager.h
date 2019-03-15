// Fill out your copyright notice in the Description page of Project Settings.

	/* Blocks bigger than 1x1 are characterised by a StartTile (A), a EndTile(B) and the zone's size for X and Y.
			_____________																_____________
			|_2_|_5_|_8_|	Given this grid, and a block where A=1, and B=8				|_x_|_x_|_x_|
			|_1_|_4_|_7_|	The block's zone would represent the "x" tiles		-->		|_x_|_x_|_x_|
			|_0_|_3_|_6_|	This zone would be of size X=1 and Y=2 						|___|___|___|

	/!\ Remember, X represent the vertical axis, Y the horizontal when it comes to tile calculation /!\ */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	* 
	*/
	void AnalyseDataBase();

	/**
	 * Resize all the tile arrays (responsible of holding tile information, such as if it is used, what is it used by etc ...)
	 * to the number of tiles there are in the level.
	 * @params NumberOfTiles Number to resize the arrays to.
	 */
	void SetTileArraysSize(int newArraySize);

	/**
	 * Clear any value in tile arrays at the specified index.
	 * @params index Index of the tile to clear (index = instance index of the tile)
	 */
	void ClearTileArraysAtIndex(int index);


	/*---------- Variables -----------*/

	/** Tile Array - Array of spawned block IDs where index = index of the tile.*/
	TArray<int> spawnedBlockID; 

	/** Tile Array - Array of spawned zone IDs where index = index of the tile.*/
	TArray<int> spawnedZoneID;

	/** Tile Array - Array of spawned blocks where index = index of the tile.*/
	TArray<ATT_Block*> spawnedBlocks;

	/** Data table holding data of all the blocks.*/
	UDataTable* data_Block;

	TArray<FTT_Struct_BlockType> blockIDArrays;

	/*/** Each element of this array contains BlockIDs of a certain block type. 
	UPROPERTY(BlueprintReadOnly)
	TArray<TArray<int>> blockIDArrays; */

	/** Each element of this array is the BlockType name of blockIDArrays. */
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> blockIDTypes;

	
public:	

	/*---------- Functions -----------*/

	/** Sets the GridManager variable and sets the size of all tile arrays.
	* @params newGridManager New value of GridManager
	*/
	void SetGridManager(ATT_GridManager* newGridManager);

	/**
	* Calculate the zone used by the block & assign the tile arrays to the block.
	* Spawn & initialize the block (passes through data table data, tile belonging to the block etc ..)
	* @params blockID Data table index of the row corresponding to the block to spawn.
	* @params blockRotation Orientation of the block. (Can only be % Pi/2 (0°, 90°, 180°)).
	 * @params tileID Index of the tile to spawn the block around.
	 */
	void SpawnBlock(int blockID, FRotator blockRotation, int tileID);

	/**
	* Gets a random blockID corresponding to parameters in the data table.
	* Calls SpawnBlock to finalise the spawning.
	* @params tileID Data table index of the row corresponding to the block.
	* @params blockRotation Orientation of the block. (Can only be % Pi/2 (0°, 90°, 180°)).
	* @params buildingType Type of building to look for (should be replaced by EBuildingType).
	* @params efficiency Level of the building to look for (1-3)
	* @params sizeX X size of block's zone (how big is the block in tiles)
	* @params sizeY Y size of block's zone (how big is the block in tiles)
	*/
	void SpawnBlockFromParameters(int tileID, FRotator blockRotation, FString buildingType, int efficiency, int sizeX, int sizeY);

	/**
	* Delete block that owns the tile selected & clears all tiles that belonged to that block. Calls DestroyBlock in the block.
	* If the tile has no block, calls DeleteZoneOnTile.
	* @params tileID TileID of the tile to clear/ owned by the block to delete.
	*/
	void DeleteBlockOnTile(int tileID);

	/**
* Assigns elements of the spawnedZoneID array to a certain ZoneID.
* @params tileIDs TileIDs of the zone's tiles.
* @params zoneID ID of the zone to assign the tiles to.
*/
	void CreateZoneOnTiles(TArray<int> tileIDs, int zoneID);

	/**
	* Clears the tile of any zoneIDs.
	* @params tileID TileID of the tile to clear of a zone.
	*/
	void DeleteZoneOnTile(int tileID);

	/**
	 * Returns all the tiles included in the zone delimited by tileA & tileB (opposing corners of the rectangular zone).
	 * (see top of page for zone explanation)
	 * @params tileA Corner A / StartTile of the zone.
	 * @params tileB Opposite corner to A.
	 */
	TArray<int> GetZoneTileIDsFromZoneParameters(int tileA, int tileB);

	/**
	 * Returns the TileID of the corner tile opposite to tileB in a zone defined by parameters (see top of page for zone explanation). 
	 * @params tileB Corner B / EndTile of the zone.
	 * @params sizeX X size of block's zone (how big is the block in tiles).
	 * @params sizeY Y size of block's zone (how big is the block in tiles).
	 * @params isModuloHalfPi If true, sizeX = sizeY & sizeY = sizeX (depending on the block's orientation).
	 */
	int GetZoneStartTileFromZoneSize(int tileB, int sizeX, int sizeY, bool isModuloHalfPi);

	/**
	 * Returns the TileID of the corner tile opposite to tileA in a zone defined by parameters (see top of page for zone explanation).
	 * @params tileA Corner A / StartTile of the zone.
	 * @params sizeX X size of block's zone (how big is the block in tiles).
	 * @params sizeY Y size of block's zone (how big is the block in tiles).
	 * @params isModuloHalfPi If true, sizeX = sizeY & sizeY = sizeX (depending on the block's orientation).
	 */
	int GetZoneEndTileFromZoneSize(int tileA, int sizeX, int sizeY, bool isModuloHalfPi);

	/**
	* Returns data of block from its BlockID (see TT_Struct_Block).
	* @params blockID Data table index of the row corresponding to the block to spawn.
	*/
	FTT_Struct_Block* GetBlockStatsFromBlockID(int blockID);

	/**
	* Returns a random blockID corresponding to parameters in the data table.
	* @params buildingType Type of building to look for (see EBuildingType).
	* @params efficiency Level of the building to look for (1-3)
	* @params sizeX X size of block's zone (how big is the block in tiles)
	* @params sizeY Y size of block's zone (how big is the block in tiles)
	*/
	int GetRandomBlockIDFromParameter(FString buildingType, int efficiency, int sizeX, int sizeY);

	/**
	* Returns an array of all the blockID corresponding to parameters in the data table.
	* @params buildingType Type of building to look for (see EBuildingType).
	* @params efficiency Level of the building to look for (1-3)
	* @params sizeX X size of block's zone (how big is the block in tiles)
	* @params sizeY Y size of block's zone (how big is the block in tiles)
	*/
	TArray<int> GetAllBlockIDsFromParameter(FString buildingType, int efficiency, int sizeX, int sizeY);

	/** Accessor - Returns the array of spawned zone where index = index of the tile.*/
	TArray<int> GetSpawnedZoneTileIDs();


	/*---------- Variables -----------*/
	
	/** Reference to the GridManager who created this block manager.*/
	ATT_GridManager* GridManager;

	/** Blueprint reference of the class of block to spawn.*/
	UPROPERTY(EditAnywhere, Category = "Block Settings")
	TSubclassOf<AActor> BlockToSpawn;
};
