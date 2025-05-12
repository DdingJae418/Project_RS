// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RSPlayerController.h"

void ARSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
