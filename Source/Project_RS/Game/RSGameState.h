// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interface/RSGameInterface.h"
#include "RSGameState.generated.h"

class ARSPlayerController;

/**
 *
 */
UCLASS()
class PROJECT_RS_API ARSGameState : public AGameStateBase, public IRSGameInterface
{
	GENERATED_BODY()

public:
	ARSGameState();

	UFUNCTION(BlueprintCallable, Category = "Game")
	float GetCurrentPlayTime() const { return CurrentPlayTime; }

	UFUNCTION(BlueprintCallable, Category = "Game")
	float GetTimeLimit() const { return TimeLimit; }

	UFUNCTION(BlueprintCallable, Category = "Game")
	bool GetIsCleared() const { return bIsCleared; }

	//~Start of Actor interface
	virtual void Tick(float DeltaSeconds) override;
	//~End of Actor interface

	//~Start of RSGameInterface interface
	virtual bool IsGameCleared() override;
	virtual void OnPlayerDead(ARSPlayerController* DeadPlayer) override;
	//~End of RSGameInterface interface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Game, Replicated)
	float TimeLimit;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Game, ReplicatedUsing = OnRep_CurrentPlayTime)
	float CurrentPlayTime;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Game, ReplicatedUsing = OnRep_IsCleared)
	uint8 bIsCleared : 1;

	UFUNCTION()
	void OnRep_CurrentPlayTime();

	UFUNCTION()
	void OnRep_IsCleared();

	//~Start of Actor interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End of Actor interface

private:
	UPROPERTY()
	TObjectPtr<class ARSPlayerController> PlayerController;
};
