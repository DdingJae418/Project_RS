// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RSPlayerController.h"
#include "UI/RSHUDWidget.h"

ARSPlayerController::ARSPlayerController()
{

}

void ARSPlayerController::PlayTimeChanged(float CurrentPlayTime)
{
	K2_OnPlayTimeChanged(CurrentPlayTime);
}

void ARSPlayerController::GameClear()
{
	K2_OnGameClear();
}

void ARSPlayerController::GameOver()
{
	K2_OnGameOver();
}

void ARSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
