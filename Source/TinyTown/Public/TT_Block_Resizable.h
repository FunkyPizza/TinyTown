// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TT_Block.h"
#include "TT_Block_Resizable.generated.h"

class UPaperGroupedSpriteComponent;
class UPaperSprite;

UCLASS()
class TINYTOWN_API ATT_Block_Resizable : public ATT_Block
{
	GENERATED_BODY()
	
public:
	ATT_Block_Resizable();

protected:

public:
	virtual void Tick(float DeltaTime) override;
};
