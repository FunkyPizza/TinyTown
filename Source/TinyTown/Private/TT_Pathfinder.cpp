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
			return 1000;
		}
		else
		{
			return 1;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("GridManager not valid in pathfinder component, cannot retrieve move cost."));
	return 1;
}

int UTT_Pathfinder::GetDistanceBetweenTwoTile(int tileA, int tileB)
{
	int distance;
	FVector locA = GridManager->GetTileLocation(tileA);
	FVector locB = GridManager->GetTileLocation(tileB);

	distance = abs(FVector::Distance(locA, locB));
	return distance;
}

TArray<int> UTT_Pathfinder::FindShortestPathDijkstra(int startTile, int goalTile)
{
	return FindShortestPathInZoneDijkstra(startTile, goalTile, tileArray);
}

TArray<int> UTT_Pathfinder::FindShortestPathInZoneDijkstra(int startTile, int goalTile, TArray<int> zone)
{
	if (zone.Num() > 1)
	{
		TArray<int> walkableTiles; // All the tiles the algo is authorised to work on.
		TArray<int> unexploredTiles; // All the tiles the algo hasn't done yet.
		TMap<int, int> distances; // Each tile's distance to StartTile (in move cost).
		TMap<int, int> parentTiles; // Each tile's preferred parent.
		TArray<int> pathResult; // All the tiles from end to start of the shortest path.
		TArray<int> EmptyTiles; // Array to return in case no paths were found.

		// Initialisation
		walkableTiles = zone;
		int tileIDOffset = startTile;
		for (int i : walkableTiles)
		{
			unexploredTiles.Add(i);
			distances.Add(i, pathfindingMaxDistance);
			parentTiles.Add(i, startTile);
		}
		distances[startTile] = 0;

		// Main looping body
		while (unexploredTiles.Num() != 0)
		{
			int currentTile = startTile;
			int minimumDistance = pathfindingMaxDistance + 1;
			TArray<int> distanceKeys;
			TArray<int> distancesValues;
			distances.GenerateValueArray(distancesValues);
			distances.GenerateKeyArray(distanceKeys);

			// Get the tileID with the shortest distance to the start tile
			for (int i = 0; i < unexploredTiles.Num(); i++)
			{
				int index = distanceKeys.Find(unexploredTiles[i]);

				if (distancesValues[index] < minimumDistance)
				{
					minimumDistance = distancesValues[index];
					currentTile = distanceKeys[index];
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
					if (distanceKeys.Contains(neighbourTileID))
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

		}

		// Getting the path from looping result
		pathResult.Add(goalTile);
		for (int i = 0; i < parentTiles.Num(); i++)
		{
			pathResult.Add(parentTiles[pathResult[i]]);
		}

		// Path is valid
		if (pathResult.Num() > 0)
		{
			return pathResult;
		}

		// Path isn't valid
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No path was found, returning empty array"));
			return EmptyTiles;
		}
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No path was found, returning empty array"));
		TArray<int> EmptyTiles;
		return EmptyTiles;
	}
}

TArray<int> UTT_Pathfinder::FindShortestPathAStar(int startTile, int goalTile)
{
	if (startTile != goalTile && GetTileMoveCost(goalTile) < 500)
	{
		TArray<int> indexedTiles;
		TArray<int> unexploredTiles;
		TMap<int, int> gCost; // Distance from start node
		TMap<int, int> hCost; // Distance to goal
		TMap<int, int> fCost; // g[i] + h[i]
		TMap<int, int> parentTiles; // Each tiles parent tile.
		TArray<int> exploredTiles;
		TArray<int> pathResult; // Final path to return
		TArray<int> EmptyTiles; // Empty path to return
		TArray<int> currentTileNeighbours;
		int currentTile;

		// Initialisation
		indexedTiles.Add(startTile);
		unexploredTiles.Add(startTile);
		parentTiles.Add(startTile, startTile);
		gCost.Add(startTile, 0);
		hCost.Add(startTile, GetDistanceBetweenTwoTile(startTile, goalTile));
		fCost.Add(startTile, gCost[startTile] + hCost[startTile]);


		// Main looping body
		while (unexploredTiles.Num() != 0)
		{
			// Get the tileID with the shortest distance to the start tile
			int lowestFCostTile = INT_MAX;
			for (int i : unexploredTiles)
			{

				if (fCost[i] < lowestFCostTile)
				{
					lowestFCostTile = fCost[i];
					currentTile = i;
				}
			}

			unexploredTiles.Remove(currentTile);
			exploredTiles.Add(currentTile);

			// Has the goal tile been reached
			if (currentTile == goalTile)
			{
				break;
			}

			// Get tile's neighbours 
			currentTileNeighbours = GetTileNeighbours(currentTile);
			for (int i = 0; i < currentTileNeighbours.Num(); i++)
			{
				int currentNeighbour = currentTileNeighbours[i];

				if (exploredTiles.Contains(currentNeighbour) || GetTileMoveCost(currentNeighbour) > 500)
				{
					// Skip to next neighbour
				}

				else
				{
					int gValue = GetDistanceBetweenTwoTile(startTile, currentNeighbour) + gCost[currentTile];
					int hValue = GetDistanceBetweenTwoTile(currentNeighbour, goalTile);
					int fValue = gValue + hValue;

					if (fCost.Contains(currentNeighbour))
					{
						if (fCost[currentNeighbour] > fValue)
						{
							gCost[currentNeighbour] = gValue;
							hCost[currentNeighbour] = hValue;
							fCost[currentNeighbour] = fValue;

							parentTiles[currentNeighbour] = currentTile;
						}
					}

					else
					{
						gCost.Add(currentNeighbour, gValue);
						hCost.Add(currentNeighbour, hValue);
						fCost.Add(currentNeighbour, fValue);

						indexedTiles.Add(currentNeighbour);
						parentTiles.Add(currentNeighbour, currentTile);
					}

					unexploredTiles.Add(currentNeighbour);
				}
			}
		}

		// Getting the path from looping result
		pathResult.Add(goalTile);
		for (int i = 0; i < parentTiles.Num(); i++)
		{
			if (pathResult[i] == startTile)
			{
				break;
			}

			pathResult.Add(parentTiles[pathResult[i]]);

		}

		// Path is valid
		if (pathResult.Num() > 2)
		{
			return pathResult;
		}

		// Path isn't valid
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No path was found, returning empty array"));
			return EmptyTiles;
		}
	}
	else
	{
	TArray<int> EmptyTiles;
	return EmptyTiles;
 }
}
