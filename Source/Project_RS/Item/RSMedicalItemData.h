// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/RSItemData.h"
#include "RSMedicalItemData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSMedicalItemData : public URSItemData
{
	GENERATED_BODY()
	
public:
	FORCEINLINE float GetRestoreAmount() const { return RestoreAmount; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Value, meta = (AllowPrivateAccess))
	float RestoreAmount;
};
