// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RSPlayerController.h"
#include "UI/RSHUDWidget.h"

ARSPlayerController::ARSPlayerController()
{
	static ConstructorHelpers::FClassFinder<URSHUDWidget> RSHUDWidgetRef(TEXT("/Game/Project_RS/UI/WBP_HUD.WBP_HUD_C"));
	if (RSHUDWidgetRef.Class)
	{
		RSHUDWidgetClass = RSHUDWidgetRef.Class;
	}
}

void ARSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	RSHUDWidget = CreateWidget<URSHUDWidget>(this, RSHUDWidgetClass);
	if (RSHUDWidget)
	{
		RSHUDWidget->AddToViewport();
	}
}
