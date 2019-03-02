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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* BuildingRoot;


/*---------- Functions -----------*/
	virtual void BeginPlay() override;


/*---------- Variables -----------*/
	UPROPERTY(BlueprintReadOnly)
	FTT_Struct_Block blockStats;

	ATT_BlockManager* blockManager;


	TArray<int> blockTileIDs;


public:	

/*---------- Components ----------*/
/*---------- Functions -----------*/
	void SetBlockStats(FTT_Struct_Block* inputStats);

	FTT_Struct_Block* GetBlockStats();

	void SetBlockManager(ATT_BlockManager* BlockManager);

	ATT_BlockManager* GetBlockManager();

	void SetBlockTileIDs(TArray<int> TileIDs);

	TArray<int> GetBlockTileIDs();

	void DestroyBlock();


/*---------- Variables -----------*/
	UPROPERTY(BlueprintReadOnly)
	int BuildTileID;
	UPROPERTY(BlueprintReadOnly)
	int BuildResizedTileID;

};
