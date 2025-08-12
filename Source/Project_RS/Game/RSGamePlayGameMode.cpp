// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RSGamePlayGameMode.h"
#include "Effect/RSEffectManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RSPlayerController.h"


ARSGamePlayGameMode::ARSGamePlayGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	TimeLimit = 10;
	CurrentPlayTime = 0;
	bIsCleared = false;
}

void ARSGamePlayGameMode::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<ARSPlayerController>(GetWorld()->GetFirstPlayerController());
	check(PlayerController);

	EffectManager = NewObject<URSEffectManager>(this);
	EffectManager->Initialize(PlayerController->GetCharacter());
}

void ARSGamePlayGameMode::Tick(float DeltaSeconds)
{
	if (bIsCleared)
		return;

	CurrentPlayTime += DeltaSeconds;
	PlayerController->PlayTimeChanged(CurrentPlayTime);
	if (TimeLimit <= CurrentPlayTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Cleared! Play Time: %f"), CurrentPlayTime);

		CurrentPlayTime = TimeLimit;
		bIsCleared = true;

		PlayerController->PlayTimeChanged(CurrentPlayTime);
		PlayerController->GameClear();
	}
}

void ARSGamePlayGameMode::OnPlayerDead()
{
	PlayerController->GameOver();
}

bool ARSGamePlayGameMode::IsGameCleared()
{
	return bIsCleared;
}
