// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RSItemData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Ammo = 0,
	Medical,
	Money,
	Story
};

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FORCEINLINE EItemType GetType() const { return Type; }
	FORCEINLINE FText GetName() const { return Name; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EItemType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Name)
	FText Name;
};
