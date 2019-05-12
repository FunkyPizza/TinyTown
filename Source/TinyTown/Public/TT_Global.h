#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TT_Global.generated.h"

class UPaperSprite;


UENUM(BlueprintType)
enum class EBlockType : uint8
{
	BT_Building 		UMETA(DisplayName = "Building"),
	BT_ZoneBuilding		UMETA(DisplayName = "Zone Building"),
	BT_Zone 			UMETA(DisplayName = "Zone"),
	BT_Path				UMETA(DisplayName = "Path"),
	BT_Nothing			UMETA(DisplayName = "Nothing")
};

/**
* This struct is used as template for the block data base. Any block (zone, building, path etc ...) has its data structured this way.
*/
USTRUCT(BlueprintType)
struct FTT_Struct_Block : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FText Block_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FString Category_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		EBlockType Block_Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		bool Resizable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 Size_X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 Size_Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FLinearColor Grid_Colour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 Efficiency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 Initial_Cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 R_Mo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 R_Po;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 R_Ha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 R_El;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 R_Wa;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 R_In;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 R_Pr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 P_Mo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 P_Po;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 P_Ha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 P_El;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 P_Wa;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 P_In;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 P_Pr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		UPaperSprite* Icon;

};

USTRUCT(BlueprintType)
struct FTT_Struct_BlockType : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Library")
		FString Type_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Library")
		TArray<int> BlockIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Library")
		TArray<FString> BlockNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Library")
		TArray<FString> BlockDescription;
	
	
	FTT_Struct_BlockType()
	{

	}

	FTT_Struct_BlockType(FString Type, TArray<int32> blockIDArray, FString blockName, FString blockDescription)
	{
		Type_Name = Type;
		BlockIDs = blockIDArray;
		BlockNames.Add(blockName);
		BlockDescription.Add(blockDescription);
	}
};


USTRUCT(BlueprintType)
struct FTT_Struct_Inventory : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 Money;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 Population;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 Happiness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 Utility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 Internet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 Production;

};
