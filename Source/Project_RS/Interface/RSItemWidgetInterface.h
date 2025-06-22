// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RSItemWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URSItemWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_RS_API IRSItemWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetupItemWidget(const FName& ItemID, int32 ItemCount) = 0;
};
