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

	// Spawns a block manager
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


	// View modes
	// Timer responsible of the view mode tile refreshing
	FTimerHandle TimerHandler_ViewMode;

	// Array of all the tiles affected by view modes
	TArray<int> viewModeTiles;

	bool isViewMode; // Indicates if any view mode is currently active
	bool isViewResidential; // Indicates if Residential view mode is active
	bool isViewCommercial; // Indicates if Commercial view mode is active
	bool isViewIndustrial; // Indicates if Industrial view mode is active

	// Colour of tiles to be displayed in view mode
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	FLinearColor ResidentialZoneTileColour;
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	FLinearColor CommercialZoneTileColour;
	UPROPERTY(EditAnywhere, Category = "Grid Settings")
	FLinearColor IndustrialZoneTileColour;



public:	

	/*---------- Components ----------*/
	UPROPERTY(VisibleDefaultsOnly)
		UPaperGroupedSpriteComponent* instanceGroupedSpriteComp;


	/*---------- Functions -----------*/

	// Set this tile as hovered -- Called by PlayGridCamera
	UFUNCTION(BlueprintCallable)
		void TileHovered(int TileID);

	//Set this tile as clicked -- Called by PlayGridCamera
	UFUNCTION(BlueprintCallable)
		void TileClicked(int TileID);

	//Set this array of tiles as Residential Zone -- Called by PlayGridCamera
	UFUNCTION(BlueprintCallable)
		void SetTileColorFromZoneID(TArray<int> TileIDs, int ZoneID);

	// Set the tile a certain color
	void SetTileColor(int TileID, FLinearColor Color);

	//Reset all altered tiles to their original state -- Called by PlayGridCamera
	UFUNCTION(BlueprintCallable)
		void TileClearState();

	//Public accessor for tile locations
	FVector GetTileLocation(int TileID);

	// Returns the distance between each tile
	float GetDistanceBetweenTiles();

	// Returns the size of the grid in a 2D Vector
	FVector2D GetGridSize();

	// Activates zone view modes (0:Residential, 1:Commercial, 2:Industrial)
	void ActivateZoneViewMode(int ViewMode);

	// Deactivates all view modes
	void StopZoneViewMode();

	// Function responsible of refreshing the tiles
	void ViewModeTick();


	/*---------- Variables -----------*/
 
	// Reference to the block manager (spawned on begin play) used by player.
	ATT_BlockManager* BlockManager; 
	   	 
};
