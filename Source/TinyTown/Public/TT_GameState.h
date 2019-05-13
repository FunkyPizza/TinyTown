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

	float timeDefault;
	int timeMultiplier;
	bool isTimeOn;

	FTT_Struct_Inventory* Inventory;
	

public:

	/*---------- Functions -----------*/

	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
	void StartTime();

	UFUNCTION(BlueprintCallable)
	void StopTime();

	void TickTime();
	UFUNCTION(BlueprintCallable)
	void SetTimeMultiplier(int multiplier);

	UFUNCTION(BlueprintImplementableEvent, Category = "Time Manager")
	void OnMinutePassed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Time Manager")
	void OnHourPassed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Time Manager")
	void OnDayPassed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Time Manager")
	void OnMonthPassed();

	void Transaction(FTT_Struct_Inventory Cost, FTT_Struct_Inventory Revenue);
	void PayFromInventory(FTT_Struct_Inventory Cost);
	void PayToInventory(FTT_Struct_Inventory Revenue);
	bool CheckTransaction(FTT_Struct_Inventory Cost);

	/*---------- Variable -----------*/
	UPROPERTY(BlueprintReadOnly)
	float Time_Seconds;
	UPROPERTY(BlueprintReadOnly)
	int Time_Minutes;
	UPROPERTY(BlueprintReadOnly)
	int Time_Hours;
	UPROPERTY(BlueprintReadOnly)
	int Time_Day;
	UPROPERTY(BlueprintReadOnly)
	int Time_Months;

	UPROPERTY(BlueprintReadOnly)
	FString Time_String;


};
