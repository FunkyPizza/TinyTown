// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_Block.generated.h"

class UDataTable;
struct FTT_Struct_Block;

UCLASS()
class TINYTOWN_API ATT_Block : public AActor
{
	GENERATED_BODY()
	
public:	

/*---------- Components ----------*/
/*---------- Functions -----------*/
	ATT_Block();


/*---------- Variables -----------*/


protected:

/*---------- Components ----------*/
/*---------- Functions -----------*/
	virtual void BeginPlay() override;

	void GetDataTable(int buildingType, int buildingID);


/*---------- Variables -----------*/
	UDataTable* data_Block;

	FTT_Struct_Block* blockStats;

public:	

/*---------- Components ----------*/
/*---------- Functions -----------*/
/*---------- Variables -----------*/

};
