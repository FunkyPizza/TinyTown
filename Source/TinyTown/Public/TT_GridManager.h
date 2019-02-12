// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_GridManager.generated.h"

class UPaperGroupedSpriteComponent;
class UPaperSprite;
class UMaterialInterface;

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
	UPROPERTY(VisibleDefaultsOnly)
		UPaperGroupedSpriteComponent* instanceGroupedSpriteComp;


	/*---------- Functions -----------*/
	virtual void BeginPlay() override;

	void SpawnTiles(int x, int y, FVector Center, float distance); // Spawns all tile instances seperated by distance in a x by y grid, centered around Center.


	/*---------- Variables -----------*/

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

	TArray<FVector> tileLocations; // Array of all spawned tile instances locations 

	int32 clickedTile; // TileID of the currently clicked tile
	TArray<int32> modifiedTiles; // Array of all tiles that have been altered


public:	

	/*---------- Functions -----------*/

	// Set this tile as hovered
	UFUNCTION(BlueprintCallable)
		void TileHovered(int TileID);

	//Set this tile as clicked
	UFUNCTION(BlueprintCallable)
		void TileClicked(int TileID);

	//Reset all altered tiles to their original state
	UFUNCTION(BlueprintCallable)
		void TileClearState();

};
