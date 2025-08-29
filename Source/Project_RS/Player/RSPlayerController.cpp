// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RSPlayerController.h"
#include "UI/RSHUDWidget.h"
#include "Character/RSCharacterPlayer.h"
#include "Project_RS.h"


ARSPlayerController::ARSPlayerController()
{
	CurrentMoney = 0;
}

void ARSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ARSCharacterPlayer* PlayerCharacter = Cast<ARSCharacterPlayer>(InPawn))
	{
		PlayerCharacter->OnOwningMoneyChanged.AddUObject(this, &ARSPlayerController::CurrentMoneyChanged);
	}
}

void ARSPlayerController::PlayTimeChanged(float CurrentPlayTime)
{
	K2_OnPlayTimeChanged(CurrentPlayTime);
}

void ARSPlayerController::GameClear()
{
	K2_OnGameClear();

	FInputModeUIOnly UIInputMode;
	SetInputMode(UIInputMode);
	SetShowMouseCursor(true);
}

void ARSPlayerController::GameOver()
{
	K2_OnGameOver();

	FInputModeUIOnly UIInputMode;
	SetInputMode(UIInputMode);
	SetShowMouseCursor(true);
}

void ARSPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ARSPlayerController::PostNetInit()
{
	Super::PostNetInit();
}

void ARSPlayerController::BeginPlay()
{
	RS_LOG(LogRSNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::BeginPlay();

	RS_LOG(LogRSNetwork, Log, TEXT("%s"), TEXT("End"));

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
