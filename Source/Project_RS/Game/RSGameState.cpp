// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RSGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/RSPlayerController.h"
#include "Kismet/GameplayStatics.h"

ARSGameState::ARSGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	TimeLimit = 180.0f;
	CurrentPlayTime = 0.0f;
	bIsCleared = false;
}

void ARSGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority() || bIsCleared)
		return;

	CurrentPlayTime += DeltaSeconds;
	OnRep_CurrentPlayTime();

	if (TimeLimit <= CurrentPlayTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Cleared! Play Time: %f"), CurrentPlayTime);

		CurrentPlayTime = TimeLimit;
		bIsCleared = true;

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (ARSPlayerController* RSPlayer = Cast<ARSPlayerController>(Iterator->Get()))
			{
				RSPlayer->GameClear();
			}
		}
	}
}

void ARSGameState::OnPlayerDead(ARSPlayerController* DeadPlayer)
{
	if (DeadPlayer)
	{
		DeadPlayer->GameOver();
	}
}

bool ARSGameState::IsGameCleared()
{
	return bIsCleared;
}

void ARSGameState::OnRep_CurrentPlayTime()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ARSPlayerController* RSPlayer = Cast<ARSPlayerController>(Iterator->Get()))
		{
			RSPlayer->PlayTimeChanged(CurrentPlayTime);
		}
	}
}

void ARSGameState::OnRep_IsCleared()
{
	if (bIsCleared)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (ARSPlayerController* RSPlayer = Cast<ARSPlayerController>(Iterator->Get()))
			{
				RSPlayer->GameClear();
			}
		}
	}
}

void ARSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARSGameState, TimeLimit);
	DOREPLIFETIME(ARSGameState, CurrentPlayTime);
	DOREPLIFETIME(ARSGameState, bIsCleared);
}
