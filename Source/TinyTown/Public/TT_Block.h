// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_Global.h"
#include "TT_Block.generated.h"


struct FTT_Struct_Block;
class ATT_BlockManager;
class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class TINYTOWN_API ATT_Block : public AActor
{
	GENERATED_BODY()
	
public:	
	ATT_Block();


protected:

/*---------- Components ----------*/

	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root;

	/** This component is used to adjust a block's position depending on its AnchorPointX (from blockStats). */
	UPROPERTY(VisibleAnywhere)
		USceneComponent* BuildingRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* BlockMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent* BoxComp;


/*---------- Functions -----------*/

	virtual void BeginPlay() override;


/*---------- Variables -----------*/

	/** This struct contains all the information that characterise this block (both block building as well a gameplay variables). */
	FTT_Struct_Block blockStats;

	/** Reference to BlockManager, set on spawn. */
	ATT_BlockManager* blockManager;

	/** This array contains all the tile the block owns. If a block is owned, no other block can be spawned on it. */
	TArray<int> blockTileIDs;


	/**
	 * The following variables are the temporary values for rotation and location of a block which is updated when in EditMode and when being spawned by BlockManager.
	 */ 
	FRotator blockRotation; 
	FVector blockAnchorLocation;

	/* Timer replicating Tick function (see EditModeTick). */
	FTimerHandle TimerHandle_EditMode;

	// Distance between tiles being set when BlockManager is set
	float distanceBetweenTiles;


public:	

/*---------- Components ----------*/

	/** This component is used to rotate a block when being placed down. */
	UPROPERTY(VisibleAnywhere)
		USceneComponent* RotationRoot;

/*---------- Functions -----------*/

// Initialisation, see variable comment for info (made in TT_BlockManager)

	void SetBlockStats(FTT_Struct_Block* inputStats);
	FTT_Struct_Block* GetBlockStats();

	UFUNCTION(BlueprintPure)
	const FTT_Struct_Block GetBlockData();

	void SetBlockManager(ATT_BlockManager* BlockManager);
	UFUNCTION(BlueprintPure)
	ATT_BlockManager* GetBlockManager();

	/* Returns a reference to the grid manager this object is using. */
	UFUNCTION(BlueprintPure)
		ATT_GridManager* GetGridManager();

	void SetCentralTileID(int tileID);

	void SetBlockTileIDs(TArray<int> TileIDs);
	TArray<int> GetBlockTileIDs();

	UFUNCTION(BlueprintPure)
		void GetOccupiedTileIDs(int32& centralTile, TArray<int32>& tileZone);

	// The two following functions only set variables, use UpdateBlockRotationAndLocation to actually confirm the move. 
	void SetBlockPosition(); // Set the block's position to fit neatly on the grid and to be centered around its anchor point. 
	void SetBlockRotation(FRotator Rotation, float blockRotaSpeed); // Set the rotator variable of the RotationRoot

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

	/** Indicates whether this block is being updated or not */
	UPROPERTY(BlueprintReadOnly, Category = "Block Settings")
	bool isInEditingMode;

	/** Rotation speed when being rotated in edit mode */
	float blockRotationSpeed;

	/** Central tile occupied by this block. */
	int centralTileID;
};
