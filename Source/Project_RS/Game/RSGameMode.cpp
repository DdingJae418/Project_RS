// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RSGameMode.h"

ARSGameMode::ARSGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerBPClassRef(TEXT("/Script/Engine.Blueprint'/Game/Project_RS/Blueprint/BP_CharacterPlayer.BP_CharacterPlayer_C'"));
	if (PlayerBPClassRef.Class)
	{
		DefaultPawnClass = PlayerBPClassRef.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find the WarriorBPClass"));
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/Project_RS.RSPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find the PlayerControllerClass"));
	}
}
