// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RSGameMode.h"

ARSGameMode::ARSGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> WarriorBPClassRef(TEXT("/Script/Project_RS.RSCharacterPlayer"));
	if (WarriorBPClassRef.Class)
	{
		DefaultPawnClass = WarriorBPClassRef.Class;
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
