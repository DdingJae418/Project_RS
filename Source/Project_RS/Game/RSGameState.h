// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RSGameState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API ARSGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	//~Start of GameStateBase interface
	virtual void HandleBeginPlay() override;
	virtual void OnRep_ReplicatedHasBegunPlay() override;
	//~End of GameStateBase interface
};
