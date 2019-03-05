// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_Global.h"
#include "TT_Block.generated.h"


struct FTT_Struct_Block;
class ATT_BlockManager;

UCLASS()
class TINYTOWN_API ATT_Block : public AActor
{
	GENERATED_BODY()
	
public:	

/*---------- Functions -----------*/
	ATT_Block();


protected:

/*---------- Components ----------*/

	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root;

	// This component is used to adjust a block's position depending on its AnchorPointX (from blockStats)
	UPROPERTY(VisibleAnywhere)
		USceneComponent* BuildingRoot;


/*---------- Functions -----------*/

	virtual void BeginPlay() override;


/*---------- Variables -----------*/

	// This struct contains all the information that characterise this block
	UPROPERTY(BlueprintReadOnly)
	FTT_Struct_Block blockStats;

	// MAY NEED TO BE REMOVED
	ATT_BlockManager* blockManager;

	// This array contains all the tile the block is covering/using.
	TArray<int> blockTileIDs;

	/* The following variables are the temporary values for rotation and location of
	a block which is updated when in EditMode and when being spawned by BlockManager.*/
	FRotator blockRotation; 
	FVector blockAnchorLocation;

	// Timer replicating Tick function (see EditModeTick)
	FTimerHandle TimerHandle_EditMode;

	// Distance between tiles being set when BlockManager is set
	float distanceBetweenTiles;


public:	

/*---------- Components ----------*/

	// This component is used to rotate a block when being placed down.
	UPROPERTY(VisibleAnywhere)
		USceneComponent* RotationRoot;

/*---------- Functions -----------*/

// Initialisation, see variable comment for info (made in TT_BlockManager)

	void SetBlockStats(FTT_Struct_Block* inputStats);
	FTT_Struct_Block* GetBlockStats();

	void SetBlockManager(ATT_BlockManager* BlockManager);
	ATT_BlockManager* GetBlockManager();

	void SetBlockTileIDs(TArray<int> TileIDs);
	TArray<int> GetBlockTileIDs();

	/* The two following functions only set variables,
		use UpdateBlockRotationAndLocation to actually confirm the move. */
	void SetBlockPosition(); // Set the block's position to fit neatly on the grid and to be centered around its anchor point. 
	void SetBlockRotation(FRotator Rotation); // Set the rotator variable of the RotationRoot

	// Updates a block's rotation and location based of 
	void UpdateBlockRotationAndLocation();

	// Activate/Stop edit mode, updating block location and rotation on tick
	void ActivateEditMode();
	void StopEditMode();
	void EditModeTick();


	// Block functions
	
	// Plays the destruction effect and destroys the block
	void DestroyBlock();


/*---------- Variables -----------*/

	// Indicates whether this block is being updated or not
	bool isInEditingMode;
};
