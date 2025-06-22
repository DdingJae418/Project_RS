// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RSCharacterBase.h"
#include "RSCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API ARSCharacterNonPlayer : public ARSCharacterBase
{
	GENERATED_BODY()
	
public:
	ARSCharacterNonPlayer();

protected:
	void SetDead() override;
};
