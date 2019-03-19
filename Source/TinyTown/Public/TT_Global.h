#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TT_Global.generated.h" 


USTRUCT(BlueprintType)
struct FTT_Struct_Block : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FText Block_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 Efficiency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		bool Resizable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 Size_X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 Size_Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FLinearColor Grid_Colour;

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

};

USTRUCT(BlueprintType)
struct FTT_Struct_BlockType : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Zone")
		FString Type_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Zone")
		TArray<int> BlockIDs;
	FTT_Struct_BlockType()
	{

	}

	FTT_Struct_BlockType(FString Type, TArray<int32> blockIDArray)
	{
		Type_Name = Type;
		BlockIDs = blockIDArray;
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

UENUM(BlueprintType)	
enum class EBuildingType : uint8
{
	BT_Residential 		UMETA(DisplayName = "Residential"),
	BT_Industrial 		UMETA(DisplayName = "Industrial"),
	BT_Commercial		UMETA(DisplayName = "Commercial"),
	BT_Entertainment	UMETA(DisplayName = "Entertainment"),
	BT_Electricity		UMETA(DisplayName = "Electricity"),
	BT_Water			UMETA(DisplayName = "Water"),
	BT_Internet			UMETA(DisplayName = "Internet")
};
