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

	/** Reset TimeMultiplier to 1 and resume counting time of day. */
	UFUNCTION(BlueprintCallable, Category = "TimeSystem")
	void StartTime();

	/** Resume counting time of day with current TimeMultiplier. */
	UFUNCTION(BlueprintCallable, Category = "TimeSystem")
	void ResumeTime();

	/** Pause current time of day. */
	UFUNCTION(BlueprintCallable, Category = "TimeSystem")
	void PauseTime();

	/**
	* Sets the speed at which the time of day is calculated. Default is 1, the high the faster time will pass.
	* @param multiplier Time multiplier.
	*/
	UFUNCTION(BlueprintCallable, Category = "TimeSystem")
	void SetTimeMultiplier(int multiplier);

	/** Functions responsible for setting all the time functions. */
	void TickTime();



	/** Is called every time a second passes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TimeSystem")
	void OnSecondPassed();
	/** Is called every time a minute passes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TimeSystem")
	void OnMinutePassed();
	/** Is called every time an hour passes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TimeSystem")
	void OnHourPassed();
	/** Is called every time a day passes */
	UFUNCTION(BlueprintImplementableEvent, Category = "TimeSystem")
	void OnDayPassed();
	/** Is called every time a month passes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TimeSystem")
	void OnMonthPassed();
	/** Is called every time a year passes. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TimeSystem")
	void OnYearPassed();


	void Transaction(FTT_Struct_Inventory Cost, FTT_Struct_Inventory Revenue);
	void PayFromInventory(FTT_Struct_Inventory Cost);
	void PayToInventory(FTT_Struct_Inventory Revenue);
	bool CheckTransaction(FTT_Struct_Inventory Cost);

	/*---------- Variable -----------*/
	UPROPERTY(BlueprintReadWrite, Category = "Time Manager")
	float Time_Seconds;
	UPROPERTY(BlueprintReadWrite, Category = "Time Manager")
	int Time_Minutes;
	UPROPERTY(BlueprintReadWrite, Category = "Time Manager")
	int Time_Hours;
	UPROPERTY(BlueprintReadWrite, Category = "Time Manager")
	int Time_Day;
	UPROPERTY(BlueprintReadWrite, Category = "Time Manager")
	int Time_Months;
	UPROPERTY(BlueprintReadWrite, Category = "Time Manager")
	int Time_Year;
	UPROPERTY(BlueprintReadOnly, Category = "Time Manager")
	FString Time_String;


};
