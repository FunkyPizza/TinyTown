#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TT_Global.generated.h" 


USTRUCT(BlueprintType)
struct FTT_Struct_Block : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 Efficiency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 Size_X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 Size_Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 Initial_Cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 R_Mo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 R_Po;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 R_Ha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 R_Ut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 R_In;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 R_Pr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 P_Mo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 P_Po;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 P_Ha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 P_Ut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 P_In;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		int32 P_Pr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Building")
		UStaticMesh* Mesh;

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