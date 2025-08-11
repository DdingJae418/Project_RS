// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RSTitlePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API ARSTitlePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ARSTitlePlayerController();

protected:
	virtual void BeginPlay() override;
};
