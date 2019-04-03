// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_GameState.h"
#include "TimerManager.h"
 #include "Engine/World.h"


/*---------- Primary functions ----------*/

void ATT_GameState::BeginPlay()
{
	timeDefault = 0.01f;
	StartTime();
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
	timeMultiplier =1;
	GetWorldTimerManager().SetTimer(TimerHandle_Time, this, &ATT_GameState::TickTime, timeDefault, true, 0.0f);
}

void ATT_GameState::StopTime()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Time);
}

void ATT_GameState::TickTime()
{
	Time_Seconds += timeMultiplier * 1000 * timeDefault;

	if (Time_Seconds >= 60)
	{
		Time_Minutes++;
		Time_Seconds = 0;
	}

	if (Time_Minutes >= 60)
	{
		Time_Hours++;
		Time_Minutes = 0;
	}
	
	if (Time_Hours >= 24)
	{
		Time_Day++;
		Time_Hours = 0;
	}

	if (Time_Day >= 31)
	{
		Time_Months++;
		Time_Day = 0;
	}

	Time_String = FString::Printf( TEXT("(%d / %d) %d : %d : %f"), Time_Day, Time_Months, Time_Hours, Time_Minutes, Time_Seconds);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Time_String);
	//UE_LOG(LogTemp, Warning, TEXT("(%d / %d) %d : %d : %f"), Time_Day, Time_Months, Time_Hours, Time_Minutes, Time_Seconds);
}

void ATT_GameState::SetTimeMultiplier(int multiplier)
{
	timeMultiplier = multiplier;
}


/*---------- Inventory functions ----------*/

void ATT_GameState::Transaction(FTT_Struct_Inventory Cost, FTT_Struct_Inventory Revenue)
{
	if (CheckTransaction(Cost))
	{
		PayFromInventory(Cost);
	}

	PayToInventory(Revenue);
}

void ATT_GameState::PayFromInventory(FTT_Struct_Inventory Cost)
{
	Inventory->Happiness -= Cost.Happiness;
	Inventory->Internet -= Cost.Internet;
	Inventory->Money -= Cost.Money;
	Inventory->Population -= Cost.Population;
	Inventory->Production -= Cost.Production;
	Inventory->Utility -= Cost.Utility;
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
