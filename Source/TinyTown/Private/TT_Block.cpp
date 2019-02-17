// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_Block.h"
#include "Engine/DataTable.h"
#include "TT_Global.h"


/*---------- Primary functions ----------*/

ATT_Block::ATT_Block()
{
	GetDataTable(0, 0);
}

void ATT_Block::BeginPlay()
{
	Super::BeginPlay();
}


/*---------- Data Table functions ----------*/

void ATT_Block::GetDataTable(int buildingType, int buildingID)
{
	//Gets Data table
	
	//TODO: Change table depending on buildingType

	static ConstructorHelpers::FObjectFinder<UDataTable> DataBlock_DataObject(TEXT("DataTable'/Game/Data/Data_Block.Data_Block'"));
	if (DataBlock_DataObject.Succeeded())
	{
		data_Block = DataBlock_DataObject.Object;

		//Gets table row
		static const FString ContextString(TEXT("data_Block_Context"));
		blockStats = data_Block->FindRow<FTT_Struct_Block>(FName(TEXT("0")), ContextString, true);
	}
}

