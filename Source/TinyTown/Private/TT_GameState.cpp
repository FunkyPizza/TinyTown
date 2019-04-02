// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_GameState.h"
#include "TimerManager.h"


/*---------- Primary functions ----------*/

void ATT_GameState::BeginPlay()
{

}

void ATT_GameState::SendNotification(FString notification)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *notification);
}

void ATT_GameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/*---------- Time functions ----------*/

void ATT_GameState::StartTime()
{
	timeMultiplier = 1;
	GetWorldTimerManager().SetTimer(TimerHandle_Time, this, &ATT_GameState::TickTime,timeMultiplier * timeDefault, true, 0.0f);
}

void ATT_GameState::StopTime()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Time);
}

void ATT_GameState::TickTime()
{
	Time_Seconds = timeDefault;

	if (Time_Seconds >= 60)
	{
		Time_Minutes++;
		Time_Seconds = 0;
	}

	if (Time_Hour >= 24)
	{
		Time_Day++;
		Time_Hour = 0;
	}
}

void ATT_GameState::StartTimeWithMultiplier(int multiplier)
{
	StopTime();
	timeMultiplier = multiplier;
	StartTime();
}

void ATT_GameState::Transaction(FTT_Struct_Inventory Cost, FTT_Struct_Inventory Revenue)
{
	if (CheckTransaction(Cost))
	{

	}

	PayToInventory(Revenue);
}

void ATT_GameState::PayToInventory(FTT_Struct_Inventory Revenue)
{
	Inventory->Happiness += Revenue.Happiness;
	Inventory->Internet += Revenue.Internet;
	Inventory->Money += Revenue.Money;
	Inventory->Population += Revenue.Population;
	Inventory->Production += Revenue.Production;
	Inventory->Utility += Revenue.Utility;
}

bool ATT_GameState::CheckTransaction(FTT_Struct_Inventory Cost)
{
	bool isTransactionPossible = true;

	if (Inventory->Happiness - Cost.Happiness > 0)
	{
		SendNotification(TEXT("Not enough happiness."));
		isTransactionPossible = false;
	}

	if (Inventory->Money - Cost.Money > 0)
	{
		SendNotification(TEXT("Not enough money."));
		isTransactionPossible = false;
	}

	if (Inventory->Internet - Cost.Internet > 0)
	{
		SendNotification(TEXT("Not enough internet."));
		isTransactionPossible = false;
	}

	if (Inventory->Population - Cost.Population > 0)
	{
		SendNotification(TEXT("Not enough population."));
		isTransactionPossible = false;
	}

	if (Inventory->Production - Cost.Production > 0)
	{
		SendNotification(TEXT("Not enough production."));
		isTransactionPossible = false;
	}

	if (Inventory->Utility - Cost.Utility > 0)
	{
		SendNotification(TEXT("Not enough utility."));
		isTransactionPossible = false;
	}

	return  isTransactionPossible;
}

/*---------- Inventory functions ----------*/