// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RSCharacterNonPlayer.h"
#include "CharacterStat/RSCharacterStatComponent.h"
#include "Enums/ECharacterName.h"
#include "AI/RSAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
#include "Physics/RSCollision.h"

ARSCharacterNonPlayer::ARSCharacterNonPlayer()
{
	AIControllerClass = ARSAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ARSCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->SetCharacterStat(CharacterName);
}

void ARSCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	ARSAIController* RSAIController = Cast<ARSAIController>(GetController());
	if (RSAIController)
	{
		RSAIController->StopAI();
	}

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), GetDeadEventDelayTime(), false);
}

float ARSCharacterNonPlayer::GetAIPatrolRadius() const
{
	return 800;
}

float ARSCharacterNonPlayer::GetAIDetectRange() const
{
	return 1000;
}

float ARSCharacterNonPlayer::GetAIAttackRange() const
{
	return Stat->GetCharacterStat().AttackRange;
}

float ARSCharacterNonPlayer::GetAITurnSpeed() const
{
	return 2.0;
}

void ARSCharacterNonPlayer::SetAIAttackDelegate(const FAICharacterAttackFinished& InDelegate)
{
	OnAttackFinished = InDelegate;
}

void ARSCharacterNonPlayer::AttackByAI()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	ProcessAttackCommand();
}

void ARSCharacterNonPlayer::NotifyAttackActionEnd()
{
	Super::NotifyAttackActionEnd();

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	OnAttackFinished.ExecuteIfBound();
}

void ARSCharacterNonPlayer::AttackHitCheck_Implementation()
{
	TArray<FOverlapResult> OutOverlapResults;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const FVector CharacterLocation = GetActorLocation();
	const FVector ForwardVector		= GetActorForwardVector();
	const float AttackRange			= Stat->GetCharacterStat().AttackRange;
	
	const FVector CapsuleCenter		= CharacterLocation + ForwardVector * (AttackRange * 0.5f);
	const float CapsuleRadius		= 50.0f;
	const float CapsuleHalfHeight	= AttackRange * 0.5f;
	const FQuat CapsuleRotation		= FQuat::FindBetweenVectors(FVector::UpVector, ForwardVector);
	FCollisionShape CapsuleShape	= FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);

	bool HitDetected = GetWorld()->OverlapMultiByChannel(
		OutOverlapResults,
		CapsuleCenter,
		CapsuleRotation,
		CCHANNEL_RSACTION,
		CapsuleShape,
		Params
	);

	if (HitDetected)
	{
		for (const FOverlapResult& OverlapResult : OutOverlapResults)
		{
			if (OverlapResult.GetActor() && OverlapResult.GetActor() != this)
			{
				FDamageEvent DamageEvent;
				OverlapResult.GetActor()->TakeDamage(Stat->GetCharacterStat().Attack, DamageEvent, GetController(), this);
			}
		}
	}

#if ENABLE_DRAW_DEBUG
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
	
	DrawDebugCapsule(
		GetWorld(),
		CapsuleCenter,
		CapsuleHalfHeight,
		CapsuleRadius,
		CapsuleRotation,
		DrawColor,
		false,
		5.0f,
		0,
		2.0f
	);

	if (HitDetected)
	{
		for (const FOverlapResult& OverlapResult : OutOverlapResults)
		{
			if (OverlapResult.GetActor() && OverlapResult.GetActor() != this)
			{
				DrawDebugPoint(GetWorld(), OverlapResult.GetActor()->GetActorLocation(), 15.0f, FColor::Yellow, false, 5.0f);
			}
		}
	}
#endif
}
