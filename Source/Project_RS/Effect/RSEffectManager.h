// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RSEffectManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSEffectManager : public UObject
{
	GENERATED_BODY()
	
public:
	URSEffectManager();

	void Initialize(ACharacter* Character);

private:
	void HandleHitEffect(AActor* HitTarget, const FHitResult& HitResult);
};
