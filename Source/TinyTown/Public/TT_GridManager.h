// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_GridManager.generated.h"

class UPaperGroupedSpriteComponent;
class UPaperSprite;

UCLASS()
class TINYTOWN_API ATT_GridManager : public AActor
{
	GENERATED_BODY()
	
public:	

	/*---------- Functions ----------*/

	ATT_GridManager();


protected:
	/*---------- Components ----------*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UPaperGroupedSpriteComponent* instanceGroupedSpriteComp;


	/*---------- Functions ----------*/

	virtual void BeginPlay() override;

	void CalculateTileLocations(int x, int y, FVector& Center, float distance);

	void SpawnTileInstances();


	/*---------- Variables ----------*/

	UPROPERTY(EditDefaultsOnly, Category ="Grid Settings")
		int sizeX;
	UPROPERTY(EditDefaultsOnly, Category = "Grid Settings")
		int sizeY;
	UPROPERTY(EditDefaultsOnly, Category = "Grid Settings")
		float distanceBetweenTiles;
	UPROPERTY(EditDefaultsOnly, Category = "Grid Settings")
		UPaperSprite* tileSpriteNormal;

	TArray<FVector> tileLocations;


public:	


};
