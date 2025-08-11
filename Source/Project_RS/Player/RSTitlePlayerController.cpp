// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RSTitlePlayerController.h"

ARSTitlePlayerController::ARSTitlePlayerController()
{

}

void ARSTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly UIInputMode;
	SetInputMode(UIInputMode);
	SetShowMouseCursor(true);

	FVector CameraLocation(2116.0f, -15.0f, 960.0f);
	FRotator CameraRotation(-6.6f, 180.0f, 0.0f);

	SetViewTarget(this);
	SetControlRotation(CameraRotation);
	GetPawn()->SetActorLocation(CameraLocation);
}
