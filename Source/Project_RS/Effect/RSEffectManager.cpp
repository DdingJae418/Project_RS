// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/RSEffectManager.h"
#include "Character/RSCharacterPlayer.h"
#include "Engine/World.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Character/RSCharacterBase.h"

URSEffectManager::URSEffectManager()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultHitEffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/Project_RS/VFX/Particles/Hit/P_Brick.P_Brick'"));
	DefaultHitEffect = DefaultHitEffectRef.Object;
	check(DefaultHitEffect);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MonsterHitEffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/Project_RS/VFX/Particles/Blood/P_Blood_Splat_Cone.P_Blood_Splat_Cone'"));
	MonsterHitEffect = MonsterHitEffectRef.Object;
	check(MonsterHitEffect);

	HitSound = nullptr;
}

void URSEffectManager::Initialize(ACharacter* Character)
{
	ARSCharacterPlayer* Player = Cast<ARSCharacterPlayer>(Character);
	check(Player);

	Player->OnHitTarget.AddUObject(this, &URSEffectManager::HandleHitEffect);
}

void URSEffectManager::HandleHitEffect(AActor* HitTarget, const FHitResult& HitResult)
{
	if (false == HitResult.bBlockingHit)
		return;

	FVector HitLocation = HitResult.Location;
	FVector HitNormal = HitResult.Normal;
	UParticleSystem* HitEffect = Cast<ARSCharacterBase>(HitTarget) ? MonsterHitEffect : DefaultHitEffect;
	PlayHitEffectAtLocation(HitLocation, HitNormal, HitEffect);
	PlayHitSound(HitLocation);
}

void URSEffectManager::PlayHitEffectAtLocation(const FVector& Location, const FVector& Normal, UParticleSystem* HitEffect)
{
	ensureMsgf(HitEffect, TEXT("HitEffect is null in URSEffectManager::PlayHitEffectAtLocation."));

	FRotator EffectRotation = FRotationMatrix::MakeFromZ(Normal).Rotator();
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		HitEffect,
		Location,
		EffectRotation,
		FVector(0.5f),
		true
	);
}

void URSEffectManager::PlayHitSound(const FVector& Location)
{
	if (!HitSound) return;

	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		HitSound,
		Location,
		1.0f,
		1.0f
	);
}
