// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TT_Global.h"
#include "TT_GameState.generated.h"


struct FTT_Struct_Inventory;

UCLASS()
class TINYTOWN_API ATT_GameState : public AGameStateBase
{
	GENERATED_BODY()
	

protected:

	/*---------- Functions -----------*/

	virtual void BeginPlay() override;

	void SendNotification(FString notification);

	/*---------- Variable -----------*/
	FTimerHandle TimerHandle_Time;

	float timeDefault = 0.1f;
	int timeMultiplier;
	bool isTimeOn;

	FTT_Struct_Inventory* Inventory;
	

public:

	/*---------- Functions -----------*/

	virtual void Tick(float DeltaTime) override;


	void StartTime();
	void StopTime();
	void TickTime();
	void StartTimeWithMultiplier(int multiplier);


	void Transaction(FTT_Struct_Inventory Cost, FTT_Struct_Inventory Revenue);
	void PayFromInventory(FTT_Struct_Inventory Cost);
	void PayToInventory(FTT_Struct_Inventory Revenue);
	bool CheckTransaction(FTT_Struct_Inventory Cost);

	/*---------- Variable -----------*/
	float Time_Seconds;
	float Time_Minutes;
	float Time_Hour;
	float Time_Day;


};
