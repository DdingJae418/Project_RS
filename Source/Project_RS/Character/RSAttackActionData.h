// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RSAttackActionData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSAttackActionData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	URSAttackActionData();

	UPROPERTY(EditAnywhere, Category = AttackData)
	float AttackRange;

	UPROPERTY(EditAnywhere, Category = AttackData)
	float AttackRadius;

	UPROPERTY(EditAnywhere, Category = AttackData)
	float AttackDamage;

	UPROPERTY(EditAnywhere, Category = ComboData)
	FString MontageSectionNamePrefix;

	UPROPERTY(EditAnywhere, Category = ComboData)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category = ComboData)
	float FrameRate;

	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount;
};
