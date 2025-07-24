// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RSCharacterAIInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URSCharacterAIInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DELEGATE(FAICharacterAttackFinished);

/**
 * 
 */
class PROJECT_RS_API IRSCharacterAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual float GetAIPatrolRadius() const = 0;
	virtual float GetAIDetectRange() const = 0;
	virtual float GetAIAttackRange() const = 0;
	virtual float GetAITurnSpeed() const = 0;

	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InDelegate) = 0;
	virtual void AttackByAI() = 0;
};
