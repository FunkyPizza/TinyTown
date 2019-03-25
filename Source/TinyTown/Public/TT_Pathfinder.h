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
	*/
	TArray<int> GetTileNeighbours(int tileID);

	/** @TODO Implement tile movement cost. */
	int GetTileMoveCost(int tileID);


	/** Reference to the GridManager. */
	ATT_GridManager* GridManager;

	/** Refers to the longest path possible the algorithm can find (in tiles). */
	int pathfindingMaxDistance;


public:	
	/** Return an array of tile IDs representing the shortest path between startTile and goalTile.
	* @param startTile TileID of the tile to start from.
	* @param goalTile TileID of the tile to end at.
	*/
	TArray<int> FindShortestPathDijkstra(int startTile, int goalTile);

	/** Return an array of tile IDs representing the shortest path between startTile and goalTile guaranteed to be in the zone.
	* @param startTile TileID of the tile to start from.
	* @param goalTile TileID of the tile to end at.
	* @param zone Array of TileIDs. The return path is guaranteed to be using only the tiles in the zone.
	*/
	TArray<int> FindShortestPathInZoneDijkstra(int startTile, int goalTile, TArray<int> zone);
		
};