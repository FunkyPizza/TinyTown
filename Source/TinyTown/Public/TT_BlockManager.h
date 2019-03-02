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

UCLASS()
class TINYTOWN_API ATT_BlockManager : public AActor
{
	GENERATED_BODY()
	
public:	

	ATT_BlockManager();


protected:
	/*---------- Functions -----------*/
	virtual void BeginPlay() override;
	
	// Initialise the block data table
	void GetBlockDataTable();

	// Sets all tile array sizes to number of tile
	void SetArraySizes();

	// Clears a specific index of all Block related arrays
	void ClearTileArraysAtIndex(int index);

	// Spawns a block at location (TileIDs is only passed through for register the block on the grid)
	void SpawnBlock(int BlockID, FTransform BlockTransform, int TileID);


	/*---------- Variables -----------*/
	// Array containing all block IDs where index=TileID
	TArray<int> spawnedBlockID; 

	// Array containing reference to all spawned blocks where index=TileID
	TArray<ATT_Block*> spawnedBlocks;

	//Reference to the "Block" DataTable 
	UDataTable* data_Block;

	
public:	

	/*---------- Functions -----------*/
	// Used to set default GridManager when spawned by ATT_GridManager
	void SetGridManager(ATT_GridManager* newGridManager);

	// Spawns block (buildingID = RowName) on TileID
	void CreateBlockOnTile(int TileID, int BuildingID, FRotator BlockRotation);

	// OVERLOAD: Spawns random block matching parameters on TileID
	void CreateBlockOnTile(int TileID, FRotator BlockRotation, FString buildingType, int efficiency, int sizeX, int sizeY);

	void DeleteBlockOnTile(int TileID);

	// Returns all the tiles included in the zone (see top of page for zone explanation)
	TArray<int> CalculateZoneTileIDs(int StartTile, int EndTile);

	// Returns the EndTileID of a zone with the given parameters (see top of page for zone explanation)
	int GetZoneEndTile(int StartTile, int SizeX, int SizeY, bool isModuloHalfPi);

	// Returns the StartTileID of a zone relative to a given anchor point (see top of page for zone explanation)
	int GetZoneStartTileFromAnchorPoint(int AnchorTileID, int AnchorPointX, int SizeX, int SizeY);

	// Returns block stats from data table by row id.
	FTT_Struct_Block* GetBlockStatsFromBlockID(int buildingID);

	// Returns a random block matching parameter in data table
	int GetRandomBlockIDFromParameter(FString buildingType, int efficiency, int sizeX, int sizeY);

	// Returns all blocks matching parameter in data table
	TArray<int> GetAllBlockIDsFromParameter(FString buildingType, int efficiency, int sizeX, int sizeY);


	/*---------- Variables -----------*/
	// Reference to the parent GridManager (who spawned this)
	ATT_GridManager* GridManager;

	//PROTOTYPE What class of actor to spawn
	UPROPERTY(EditAnywhere, Category = "Block Settings")
	TSubclassOf<AActor> BlockToSpawn;


};
