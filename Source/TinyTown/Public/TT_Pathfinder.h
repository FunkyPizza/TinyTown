// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TT_Pathfinder.generated.h"

class ATT_GridManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TINYTOWN_API UTT_Pathfinder : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTT_Pathfinder();

protected:

	virtual void BeginPlay() override;

	/**
	* Finds and saves the GridManager as a reference.
	* This uses the first existing GridManager object that it finds, doesn't support multiple GridManager objects.
	*/
	ATT_GridManager* GetGridManager();

	/** Returns the tile's neighbours in a clockwise order (Right, Down, Left, Up). 
	* @param tileID Specified tileID.
	* @param allowDiagonalPaths Include the diagonal neighbours
	*/
	UFUNCTION(BlueprintCallable, Category = "Pathfinder")
	TArray<int> GetTileNeighbours(int tileID, bool allowDiagonalPaths);

	/* Returns the cost of moving on that tile. 
	* @param tileID			Tile to return of the cost of.
	* @param blockToIgnore	Block ID of the block to ignore.
	*/
	int GetTileMoveCost(int tileID, int blockToIgnore);

	/* Returns the distance in Unreal unit between two tiles.
	* @param tileA Tile of reference.
	* @param tileB Tile to measure the distance to.
	*/
	UFUNCTION(BlueprintCallable, Category = "Pathfinder")
	int GetDistanceBetweenTwoTile(int tileA, int tileB);


	/** Reference to the GridManager. */
	ATT_GridManager* GridManager;

	/** Array of all the tile on the grid, is used for classic Dijkstra. */
	TArray<int> tileArray;

	/** Refers to the longest path possible the algorithm can find (in tiles). */
	int pathfindingMaxDistance;


public:	
	/** Return an array of tile IDs representing the shortest path between startTile and goalTile using all of the grid's tiles.
	* /!\ Very demanding on resources, WILL kill your FPS. Only use on tiny grids. /!\
	* @param startTile			 TileID of the tile to start from.
	* @param goalTile			 TileID of the tile to end at.
	* @param allowDiagonalPaths  Allow the algorithm to use diagonal paths.
	* @param blockToIgnore		 Allow the algorithm to ignore certain blocks. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Pathfinder")
	TArray<int> FindShortestPathDijkstra(int startTile, int goalTile, bool allowDiagonalPaths, int blockToIgnore);

	/** Return an array of tile IDs representing the shortest path between startTile and goalTile guaranteed to be in the zone.
	* @param							startTile TileID of the tile to start from.
	* @param							goalTile TileID of the tile to end at.
	* @param zone Array of TileIDs.		The return path is guaranteed to be using only the tiles in the zone.
	* @param allowDiagonalPaths			Allow the algorithm to use diagonal paths.
	* @param blockToIgnore				Allow the algorithm to ignore certain blocks.
	*/
	UFUNCTION(BlueprintCallable, Category = "Pathfinder")
	TArray<int> FindShortestPathInZoneDijkstra(int startTile, int goalTile, TArray<int> zone, bool allowDiagonalPaths, int blockToIgnore);
	
	/**
	* Return an array of tile IDs representing the shortest path between startTile and goalTile using A* pathfinding.
	* @param						startTile TileID of the tile to start from.
	* @param						goalTile TileID of the tile to end at.
	* @param allowDiagonalPaths		Allow the algorithm to use diagonal paths.
	* @param blockToIgnore			Allow the algorithm to ignore certain blocks.
	*/
	UFUNCTION(BlueprintCallable, Category = "Pathfinder")
	TArray<int> FindShortestPathAStar(int startTile, int goalTile, bool allowDiagonalPaths, int blockToIgnore);
};
