// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RSCharacterItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URSCharacterItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_RS_API IRSCharacterItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void FindItem(class ARSItem* InItem) = 0;
	virtual void LoseItem(class ARSItem* InItem) = 0;
};
