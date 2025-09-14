// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RSGamePlayGameMode.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_RS_API ARSGamePlayGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ARSGamePlayGameMode();

	//~Start of Actor interface
	virtual void BeginPlay() override;
	//~End of Actor interface
	
	//~Start of GameModeBase interface
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;
	//~End of GameModeBase interface


protected:

private:
	UPROPERTY()
	TObjectPtr<class URSEffectManager> EffectManager;

	UPROPERTY()
	TObjectPtr<class ARSPlayerController> PlayerController;
};
