// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RSGamePlayGameMode.h"
#include "Effect/RSEffectManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RSPlayerController.h"
#include "RSGameState.h"


ARSGamePlayGameMode::ARSGamePlayGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	TimeLimit = 180;
	CurrentPlayTime = 0;
	bIsCleared = false;

	GameStateClass = ARSGameState::StaticClass();
	EffectManager = CreateDefaultSubobject<URSEffectManager>(TEXT("EffectManager"));
}

void ARSGamePlayGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ARSGamePlayGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsCleared)
		return;

	CurrentPlayTime += DeltaSeconds;
	
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ARSPlayerController* RSPlayer = Cast<ARSPlayerController>(Iterator->Get()))
		{
			RSPlayer->PlayTimeChanged(CurrentPlayTime);
		}
	}
	
	if (TimeLimit <= CurrentPlayTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Cleared! Play Time: %f"), CurrentPlayTime);

		CurrentPlayTime = TimeLimit;
		bIsCleared = true;

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (ARSPlayerController* RSPlayer = Cast<ARSPlayerController>(Iterator->Get()))
			{
				RSPlayer->PlayTimeChanged(CurrentPlayTime);
				RSPlayer->GameClear();
			}
		}
	}
}

void ARSGamePlayGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

APlayerController* ARSGamePlayGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	return NewPlayerController;
}

void ARSGamePlayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ARSPlayerController* RSPlayer = Cast<ARSPlayerController>(NewPlayer))
	{
		if (!PlayerController)
		{
			PlayerController = RSPlayer;
			if (EffectManager && RSPlayer->GetCharacter())
			{
				EffectManager->Initialize(RSPlayer->GetCharacter());
				UE_LOG(LogTemp, Log, TEXT("Effect Manager Initialized."));
			}
		}
	}
}

void ARSGamePlayGameMode::StartPlay()
{
	Super::StartPlay();
}

void ARSGamePlayGameMode::OnPlayerDead()
{
	if (PlayerController)
	{
		PlayerController->GameOver();
	}
}

bool ARSGamePlayGameMode::IsGameCleared()
{
	return bIsCleared;
}
