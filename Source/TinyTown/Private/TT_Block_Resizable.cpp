// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_Block_Resizable.h"
#include "PaperGroupedSpriteComponent.h"
#include "TT_BlockManager.h"
#include "TT_GridManager.h"

ATT_Block_Resizable::ATT_Block_Resizable()
{

	/*---------- Setting defaults ----------*/
	PrimaryActorTick.bCanEverTick = true;
}

void ATT_Block_Resizable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


