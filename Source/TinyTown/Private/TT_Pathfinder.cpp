// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_Pathfinder.h"
#include "TT_GridManager.h"
#include "EngineUtils.h"
#include "TT_GridManager.h"

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
	return 1;
}

TArray<int> UTT_Pathfinder::FindShortestPathDijkstra(int startTile, int goalTile)
{
	return FindShortestPathInZoneDijkstra(startTile, goalTile, GridManager->GetAllTileIDs());
}

TArray<int> UTT_Pathfinder::FindShortestPathInZoneDijkstra(int startTile, int goalTile, TArray<int> zone)
{
	TArray<int> unexploredTiles;

	TArray<int> walkableTiles = zone;

	TMap<int, int> distances;

	TMap<int, int> parentTiles;

	int tileIDOffset = startTile;


	for (auto i : walkableTiles)
	{
		unexploredTiles.Add(i);
		distances.Add(i, pathfindingMaxDistance);
		parentTiles.Add(i,0);
	}


	distances[startTile] = 0;


	while (unexploredTiles.Num() > 0)
	{
		// Get the tileID with the shortest distance to the start tile
		int currentTile;
		TArray<int> distanceKeys;
		TArray<int> distancesValues;
		distances.GenerateValueArray(distancesValues);
		distances.GenerateKeyArray(distanceKeys);
		for (int i = 0; i < distancesValues.Num(); i++)
		{
			int minimumDistance = pathfindingMaxDistance;

			if (distancesValues[i] < minimumDistance)
			{
				minimumDistance = distancesValues[i];
				currentTile = distanceKeys[i];
			}
		}

		// Check if is goal
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

	UE_LOG(LogTemp, Warning, TEXT("No path was found, returning empty array"));
	TArray<int> EmptyTiles;
	return EmptyTiles;
}

