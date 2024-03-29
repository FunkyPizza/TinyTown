#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TT_Global.generated.h"

class UPaperSprite;
class ATT_Block;

/** This enum is used to differentiate between different type of buildings. */
UENUM(BlueprintType)
enum class EBlockType : uint8
{
	BT_Building 		UMETA(DisplayName = "Building", ToolTip = "A building is placed down with one click and can be rotated."),
	BT_ZoneBuilding		UMETA(DisplayName = "Zone Building", ToolTip = "A zone building is automatically spawned when creating zones."),
	BT_Zone 			UMETA(DisplayName = "Zone", ToolTip = "A zone is placed down by dragging the mouse over several tiles and spawns Zone Buildings."),
	BT_Path				UMETA(DisplayName = "Path", ToolTip = "A path is placed down by dragging the mouse from one tile to another, will spawn on the shortest path between the two."),
	BT_Nothing			UMETA(DisplayName = "Nothing", ToolTip = "Will do nothing.")
};

/** This struct is used to store all the relevant data to identify a block. */
USTRUCT(BlueprintType)
struct FTT_Struct_Block : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

		/** Name of the block. For example, a block can be called "Town Hall". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FText Block_Name;

	/** Description of the block. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FText Description;

	/** Category used to organise all different type of buildings. This is for example used to organise the building widget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FString Category_Name;


	/** How many tiles on the X axis should this building take. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 Size_X;

	/** How many tiles on the Y axis should this building take. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		int32 Size_Y;

	/** Defines how the game should do with this block. See enum description for more info on the types. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		EBlockType Block_Type;

	/** Icon to represent this block in UI widgets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		UPaperSprite* Icon;

	/** Each block should have its own child class of TT_Block. Use this to assign a block to its class. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		TSubclassOf<ATT_Block> BlockClass;

	/** Not properly implemented yet. So far, only used for viewmode filters. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FStruct_Block")
		FLinearColor Grid_Colour;

};

/** This struct is used to organise block categories. */
USTRUCT(BlueprintType)
struct FTT_Struct_BlockType : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Library")
		FString Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Library")
		TArray<FString> Block_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block Library")
		TArray<int> BlockIDs;

	
	
	FTT_Struct_BlockType()
	{
	}

	FTT_Struct_BlockType(FString Type, TArray<FString> blockName, TArray<int32> blockIDArray)
	{
		Category = Type;
		Block_Name = blockName;
		BlockIDs = blockIDArray;
	}
};

/** This struct is used for transactions. */
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
