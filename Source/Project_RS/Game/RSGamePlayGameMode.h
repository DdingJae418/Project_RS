// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/RSGameInterface.h"
#include "RSGamePlayGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API ARSGamePlayGameMode : public AGameModeBase, public IRSGameInterface
{
	GENERATED_BODY()
	
public:
	ARSGamePlayGameMode();

	//~Start of Actor interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	//~End of Actor interface
	
	//~Start of GameModeBase interface
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;
	//End of GameModeBase interface

	//~Start of RSGameInterface interface
	virtual void OnPlayerDead() override;
	virtual bool IsGameCleared() override;
	//~End of RSGameInterface interface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Game)
	float TimeLimit;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Game)
	float CurrentPlayTime;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Game)
	uint8 bIsCleared : 1;

private:
	UPROPERTY()
	TObjectPtr<class URSEffectManager> EffectManager;

	UPROPERTY()
	TObjectPtr<class ARSPlayerController> PlayerController;
};
