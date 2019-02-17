// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_GridManager.generated.h"

class UPaperGroupedSpriteComponent;
class UPaperSprite;
class UMaterialInterface;
class ATT_BlockManager;

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

	// Spawns all tile instances separated by distance in a x by y grid, centered around Center.
	void SpawnTiles(int x, int y, FVector Center, float distance); 

	void SpawnBlockManager();


	/*---------- Variables -----------*/

	// Class of block manager to spawn
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		TSubclassOf<ATT_BlockManager> BlockManagerClass;

	//Size of the grid on the X axis
	UPROPERTY(EditAnywhere, Category ="Grid Settings")
		int sizeX;

	//Size of the grid on the Y axis
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		int sizeY;

	//Distance that separate each tile from one another
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		float distanceBetweenTiles;

	//Default sprite for a tile
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		UPaperSprite* tileSpriteNormal;

	//Z offset for hovered tiles
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
		float tileHoveredZOffset;


	// Array of all spawned tile instances locations
	TArray<FVector> tileLocations; 

	// TileID of the currently clicked tile
	int32 clickedTile; 	

	// Array of all tiles that have been altered
	TArray<int32> modifiedTiles; 


public:	
	UPROPERTY(VisibleDefaultsOnly)
		UPaperGroupedSpriteComponent* instanceGroupedSpriteComp;
	/*---------- Functions -----------*/

	// Set this tile as hovered -- Called by PlayGridCamera
	UFUNCTION(BlueprintCallable)
		void TileHovered(int TileID);

	//Set this tile as clicked -- Called by PlayGridCamera
	UFUNCTION(BlueprintCallable)
		void TileClicked(int TileID);

	//Reset all altered tiles to their original state -- Called by PlayGridCamera
	UFUNCTION(BlueprintCallable)
		void TileClearState();

	//Public accessor for tile locations
	FVector GetTileLocation(int TileID);

	/*---------- Variables -----------*/
 
	//Reference to the block manager (spawned on begin play) used by player.
	ATT_BlockManager* BlockManager; 



};
