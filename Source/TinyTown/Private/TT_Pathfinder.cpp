// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_Pathfinder.h"
#include "TT_GridManager.h"
#include "EngineUtils.h"
#include "TT_GridManager.h"
#include "TT_BlockManager.h"

// Sets default values for this component's properties
UTT_Pathfinder::UTT_Pathfinder()
{
	pathfindingMaxDistance = 500;
}


// Called when the game starts
void UTT_Pathfinder::BeginPlay()
{
	Super::BeginPlay();

	GridManager = GetGridManager();	
	if (!GridManager)
	{
		UE_LOG(LogTemp, Error, TEXT("GridManager isn't valid in pathfinder component. Pathfinding won't work."))
	}

	tileArray = GridManager->GetAllTileIDs();
}

ATT_GridManager* UTT_Pathfinder::GetGridManager()
{
	// Iterate through actors of class TT_GridManager and gets first result. (There should be only one at all times)
	for (TActorIterator<ATT_GridManager> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		if (ActorItr->IsA(ATT_GridManager::StaticClass()))
		{
			return *ActorItr;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Couldn't get GridManager in Pathfinder Component."))
		return nullptr;
}

TArray<int> UTT_Pathfinder::GetTileNeighbours(int tileID)
{
	TArray<int> neighbours;
	FVector2D gridSize = GridManager->GetGridSize();
	int currentNeighbour;

	currentNeighbour = tileID + gridSize.X;
	if (GridManager->IsTileValid(currentNeighbour))
	{
		neighbours.Add(currentNeighbour);
	}

	currentNeighbour = tileID - 1;
	if (GridManager->IsTileValid(currentNeighbour))
	{
		neighbours.Add(currentNeighbour);
	}

	currentNeighbour = tileID - gridSize.X;
	if (GridManager->IsTileValid(currentNeighbour))
	{
		neighbours.Add(currentNeighbour);
	}

	currentNeighbour = tileID + 1;
	if (GridManager->IsTileValid(currentNeighbour))
	{
		neighbours.Add(currentNeighbour);
	}

	return neighbours;
}

int UTT_Pathfinder::GetTileMoveCost(int tileID)
{
	if (GridManager)
	{
		if (GridManager->BlockManager->GetSpawnedBlockIDs()[tileID] != 0)
		{
			return 100;
		}
		else
		{
			return 1;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("GridManager not valid in pathfinder component, cannot retrieve move cost."));
	return 1;
}

TArray<int> UTT_Pathfinder::FindShortestPathDijkstra(int startTile, int goalTile)
{
	return FindShortestPathInZoneDijkstra(startTile, goalTile, tileArray);
}


TArray<int> UTT_Pathfinder::FindShortestPathInZoneDijkstra(int startTile, int goalTile, TArray<int> zone)
{
	TArray<int> walkableTiles = zone;
	TArray<int> unexploredTiles;
	TMap<int, int> distances;
	TMap<int, int> parentTiles;

	int tileIDOffset = startTile;

	for (int i : walkableTiles)
	{
		unexploredTiles.Add(i);
		distances.Add(i, pathfindingMaxDistance);
		parentTiles.Add(i, startTile);
	}

	distances[startTile] = 0;

	while (unexploredTiles.Num() != 0)
	{
		// Get the tileID with the shortest distance to the start tile
		int currentTile = startTile;
		int minimumDistance = pathfindingMaxDistance + 1;
		TArray<int> distanceKeys;
		TArray<int> distancesValues;
		distances.GenerateValueArray(distancesValues);
		distances.GenerateKeyArray(distanceKeys);

		for (int i = 0; i < distancesValues.Num(); i++)
		{
			if (distancesValues[i] < minimumDistance)
			{
				minimumDistance = distancesValues[i];
				currentTile = distanceKeys[i];
			}
		}

		/*// Check if is goal
		if (currentTile == goalTile)
		{
			UE_LOG(LogTemp, Warning, TEXT("Goal reached!"));

			// Get path into an array of Tile IDs
			TArray<int> pathResult;
			pathResult.Add(currentTile);

			// Get all the parent tiles starting from the goal line
			for (int i = 0; i < parentTiles.Num(); i++)
			{
				if (currentTile == startTile)
				{
					return pathResult;
				}

				int parentTile = parentTiles[pathResult[i]];
				pathResult.Add(parentTile);

				// If the parent tile is the start tile we have reached the start of the path
				if (parentTile ==  startTile)
				{
					return pathResult;
				}
			}
		}
		*/

		// Remove the tile from unexploredTiles
		unexploredTiles.Remove(currentTile);

		// Get tile's neighbours 
		TArray<int> tileNeighbours = GetTileNeighbours(currentTile);

		for (int i = 0; i < tileNeighbours.Num(); i++)
		{
			int neighbourTileID = tileNeighbours[i];

			if (walkableTiles.Contains(neighbourTileID))
			{
				int dist = 0;
				dist = distances[currentTile] + GetTileMoveCost(neighbourTileID);

				if (dist < distances[neighbourTileID])
				{
					distances[neighbourTileID] = dist;
					parentTiles[neighbourTileID] = currentTile;
				}
			}
		}
	}

	TArray<int> pathResult;
	pathResult.Add(goalTile);

	for (int i = 0; i < parentTiles.Num(); i++)
	{
		pathResult.Add(parentTiles[pathResult[i]]);
	}

	if (pathResult.Num() > 0)
	{
		return pathResult;
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No path was found, returning empty array"));
		TArray<int> EmptyTiles;
		return EmptyTiles;
	}
}


/*
TArray<int> UTT_Pathfinder::FindShortestPathInZoneDijkstra(int startTile, int goalTile, TArray<int> zone)
{
	TArray<int> walkableTiles = zone;
	int zoneSize = walkableTiles.Num();

	TArray<int> unexploredTiles;
	unexploredTiles.SetNum(zoneSize);

	TArray<int> distances;
	distances.SetNum(zoneSize);

	TArray<int> parentTiles;
	parentTiles.SetNum(zoneSize);

	int tileIDOffset = startTile;


	for (int i = 0; i < zoneSize -1; i++)
	{
		unexploredTiles[i] = walkableTiles[i];
		distances[i] = pathfindingMaxDistance;
		parentTiles[i] = 0;
	}

	distances[walkableTiles.Find(startTile)] = 0;


	while (unexploredTiles.Num() > 0)
	{
		// Get the tileID with the shortest distance to the start tile
		int currentTile;

		for (int i = 0; i < unexploredTiles.Num(); i++)
		{
			int minimumDistance = pathfindingMaxDistance + 1;

			if (distances[i] < minimumDistance)
			{
				minimumDistance = distances[i];
				currentTile = unexploredTiles[i];
			}
		}


		// Remove the tile from unexploredTiles
		unexploredTiles.Remove(currentTile);

		// If is start tile don't do anything
		if (currentTile == startTile)
		{
			break;
		}

		// Get tile's neighbours 
		TArray<int> tileNeighbours = GetTileNeighbours(currentTile);

		for (int i = 0; i < tileNeighbours.Num(); i++)
		{
			int neighbourTileID = tileNeighbours[i];

			if (walkableTiles.Contains(neighbourTileID))
			{
				int dist = 0;
				dist = distances[walkableTiles.Find(currentTile)] + GetTileMoveCost(neighbourTileID);

				if (dist < distances[walkableTiles.Find(neighbourTileID)])
				{
					distances[walkableTiles.Find(neighbourTileID)] = dist;
					parentTiles[walkableTiles.Find(neighbourTileID)] = currentTile;
				}
			}
		}
	}

	TArray<int> pathResult;
	pathResult.Add(goalTile);

	
	for (int i = 0; i < parentTiles.Num() - 1; i++)
	{
		pathResult.Add( parentTiles[ walkableTiles.Find( pathResult[i] ) ] );
	}

	if (pathResult.Num() > 0)
	{
		return pathResult;
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No path was found, returning empty array"));
		TArray<int> EmptyTiles;
		return EmptyTiles;
	}
}
*/
