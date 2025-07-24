// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RSAnimationAttackInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URSAnimationAttackInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for animation-triggered attack functionality.
 * Allows animation notifies to trigger attack hit checking.
 */
class PROJECT_RS_API IRSAnimationAttackInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void AttackHitCheck();
	virtual void AttackHitCheck_Implementation() = 0;
};
