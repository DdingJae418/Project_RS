// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/RSItemData.h"
#include "RSAmmoItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSAmmoItemData : public URSItemData
{
	GENERATED_BODY()

public:
	FORCEINLINE float GetAmmoAmount() const { return AmmoAmount_; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Value, meta = (AllowPrivateAccess))
	float AmmoAmount_;
};
