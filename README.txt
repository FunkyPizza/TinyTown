/*---------- Components ----------*/
/*---------- Functions -----------*/
/*---------- Variables -----------*/

TArray<int> UTT_Pathfinder::FindShortestPathAStar(int startTile, int goalTile)
{
	if (startTile != goalTile)
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
		parentTiles.Add(startTile);
		gCost.Add(0);
		hCost.Add(GetDistanceBetweenTwoTile(startTile, goalTile));
		fCost.Add(gCost[0] + hCost[0]);


		// Main looping body
		while (unexploredTiles.Num() != 0)
		{
			// Get the tileID with the shortest distance to the start tile
			for (int i = 0; i < unexploredTiles.Num(); i++)
			{
				int lowestFCostTile = INT_MAX;

				if (fCost[i] < lowestFCostTile)
				{
					lowestFCostTile = fCost[i];
					currentTile = unexploredTiles[i];
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
					bool newPathIsShorter = false;
					int gValue = GetDistanceBetweenTwoTile(startTile, currentNeighbour);
					int hValue = GetDistanceBetweenTwoTile(currentNeighbour, goalTile);

					if (indexedTiles.Contains(currentNeighbour))
					{
						int i = indexedTiles.Find(currentNeighbour);

						if (fCost[i] < gValue + hValue)
						{
							gCost[i] = gValue;
							hCost[i] = hValue;
							fCost[i] = gValue + hValue;

							parentTiles[i] = currentTile;
						}
					}
	
					else if (!unexploredTiles.Contains(currentNeighbour))
					{
						gCost.Add(gValue);
						hCost.Add(hValue);
						fCost.Add(gValue + hValue);

						indexedTiles.Add(currentNeighbour);
						parentTiles.Add(currentTile);

						if (!unexploredTiles.Contains(currentNeighbour))
						{
							unexploredTiles.Add(currentNeighbour);
						}
					}
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

			pathResult.Add(parentTiles[indexedTiles.Find(pathResult[i])]);

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
	TArray<int> EmptyTiles;
	return EmptyTiles;
 }
}
