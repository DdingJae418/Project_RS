// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/RSItemData.h"
#include "RSMoneyItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSMoneyItemData : public URSItemData
{
	GENERATED_BODY()
	
public:
	FORCEINLINE float GetMoneyValue() const { return MoneyValue; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Value, meta = (AllowPrivateAccess))
	float MoneyValue;
};
