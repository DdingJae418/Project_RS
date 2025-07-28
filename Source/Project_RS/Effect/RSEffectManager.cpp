// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/RSEffectManager.h"
#include "Character/RSCharacterPlayer.h"

URSEffectManager::URSEffectManager()
{

}

void URSEffectManager::Initialize(ACharacter* Character)
{
	ARSCharacterPlayer* Player = Cast<ARSCharacterPlayer>(Character);
	check(Player);

	Player->OnHitTarget.AddUObject(this, &URSEffectManager::HandleHitEffect);
}

void URSEffectManager::HandleHitEffect(AActor* HitTarget, const FHitResult& HitResult)
{
	UE_LOG(LogTemp, Log, TEXT("HIt!"));
}
