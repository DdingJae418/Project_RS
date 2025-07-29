// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/World.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effects")
	TObjectPtr<class UParticleSystem> DefaultHitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effects")
	TObjectPtr<class UParticleSystem> MonsterHitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effects")
	TObjectPtr<class USoundCue> HitSound;

private:
	void HandleHitEffect(AActor* HitTarget, const FHitResult& HitResult);

	void PlayHitEffectAtLocation(const FVector& Location, const FVector& Normal, UParticleSystem* HitEffect);
	void PlayHitSound(const FVector& Location);
};
