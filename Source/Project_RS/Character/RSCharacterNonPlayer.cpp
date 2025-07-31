// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RSCharacterNonPlayer.h"
#include "CharacterStat/RSCharacterStatComponent.h"
#include "Enums/ECharacterName.h"
#include "AI/RSAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
#include "Physics/RSCollision.h"
#include "UI/RSWidgetComponent.h"
#include "UI/RSHpBarWidget.h"
#include "Components/CapsuleComponent.h"

ARSCharacterNonPlayer::ARSCharacterNonPlayer()
{
	AIControllerClass = ARSAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Widget Component
	HpBar = CreateDefaultSubobject<URSWidgetComponent>(TEXT("Widget"));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/Project_RS/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(100.f, 10.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HpBar->SetHiddenInGame(true);
	}
}

void ARSCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->SetCharacterStat(CharacterName);
	SetupHpBarComponent();
}

void ARSCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	ARSAIController* RSAIController = Cast<ARSAIController>(GetController());
	if (RSAIController)
	{
		RSAIController->StopAI();
	}

	if (HpBar != nullptr)
		HpBar->SetHiddenInGame(true);

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
	PlayAttackSound();

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

float ARSCharacterNonPlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HpBar != nullptr && HpBar->bHiddenInGame && KINDA_SMALL_NUMBER < Stat->GetCurrentHp())
	{
		HpBar->SetHiddenInGame(false);
	}

	return DamageAmount;
}

void ARSCharacterNonPlayer::SetupWidget(class UUserWidget* InUserWidget)
{
	URSHpBarWidget* HpBarWidget = Cast<URSHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		HpBarWidget->SetMaxHp(Stat->GetCharacterStat().MaxHp);
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &URSHpBarWidget::UpdateHpBar);
	}
}

void ARSCharacterNonPlayer::SetupHpBarComponent()
{
	const FName SocketName = TEXT("headSocket");

	if (GetMesh() && GetMesh()->DoesSocketExist(SocketName))
	{
		HpBar->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, SocketName);
		HpBar->SetWorldLocation(GetMesh()->GetSocketLocation(SocketName) + FVector(0.0f, 0.0f, 30.0f));
	}
	else
	{
		HpBar->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform);
		FVector CharacterTop = GetActorLocation() + FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 50.0f);
		HpBar->SetWorldLocation(CharacterTop);
	}
}
