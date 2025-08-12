// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RSPlayerController.h"
#include "UI/RSHUDWidget.h"
#include "Character/RSCharacterPlayer.h"


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

void ARSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
