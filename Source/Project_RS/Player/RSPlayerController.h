// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API ARSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARSPlayerController();

	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnPlayTimeChangedCpp"))
	void K2_OnPlayTimeChanged(float CurrentPlayTime);

	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGameClearCpp"))
	void K2_OnGameClear();

	UFUNCTION(BlueprintImplementableEvent, Category = Game, Meta = (DisplayName = "OnGameOverCpp"))
	void K2_OnGameOver();

	UFUNCTION()
	void CurrentMoneyChanged(uint32 NewMoney) { CurrentMoney = NewMoney; }

	void PlayTimeChanged(float CurrentPlayTime);
	void GameClear();
	void GameOver();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int64 CurrentMoney;
};
