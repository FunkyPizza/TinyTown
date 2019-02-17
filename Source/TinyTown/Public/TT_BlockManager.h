// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_BlockManager.generated.h"

class ATT_GridManager;

UCLASS()
class TINYTOWN_API ATT_BlockManager : public AActor
{
	GENERATED_BODY()
	
public:	

	/*---------- Components ----------*/

	/*---------- Functions -----------*/
	ATT_BlockManager();


	/*---------- Variables -----------*/



protected:

	/*---------- Components ----------*/

	/*---------- Functions -----------*/

	virtual void BeginPlay() override;




	/*---------- Variables -----------*/

	// Reference to the parent GridManager (who spawned this)
	ATT_GridManager* GridManager; 


public:	

	void CreateBuildingOnTile(int TileID);

	// Used to set default GridManager when spawned by ATT_GridManager
	void SetGridManager(ATT_GridManager* newGridManager); 

	UPROPERTY(EditAnywhere, Category = "Block Settings")
	TSubclassOf<AActor> BlockToSpawn;

	AActor* SpawnedActor;
};
