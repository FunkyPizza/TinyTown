// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TT_Global.h"
#include "TT_Block.h"
#include "TT_GhostBlock.generated.h"

struct FTT_Struct_Block;

UCLASS()
class TINYTOWN_API ATT_GhostBlock : public ATT_Block
{
	GENERATED_BODY()
	
public:	

	ATT_GhostBlock();

protected:
	
	virtual void BeginPlay() override;


public:	

};
