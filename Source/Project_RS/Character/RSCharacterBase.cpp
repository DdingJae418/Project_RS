// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RSCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RSCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "RSAttackActionData.h"
#include "Physics/RSCollision.h"
#include "CharacterStat/RSCharacterStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ARSCharacterBase::ARSCharacterBase()
{
	// Pawn
	bUseControllerRotationPitch	= false;
	bUseControllerRotationYaw	= false;
	bUseControllerRotationRoll	= false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_RSCAPSULE);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->MaxAcceleration = 1024.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 400.f;
	GetCharacterMovement()->bUseSeparateBrakingFriction = true;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// Stat Component
	Stat = CreateDefaultSubobject<URSCharacterStatComponent>(TEXT("Stat"));
	Stat->OnHpZero.AddUObject(this, &ARSCharacterBase::SetDead);

}

void ARSCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


void ARSCharacterBase::SetCharacterControlData(const class URSCharacterControlData* CharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void ARSCharacterBase::ProcessAttackCommand()
{
	if (IsDead_)
		return;

	if (0 == CurrentCombo_)
	{
		AttackActionBegin();
		return;
	}

	if (false == ComboTimerHandle_.IsValid())
	{
		HasNextAttackCommand_ = false;
	}
	else
	{
		HasNextAttackCommand_ = true;
	}
}

void ARSCharacterBase::AttackActionBegin()
{
	CurrentCombo_ = 1;

	const float AttackSpeedRate = Stat->GetCharacterStat().AttackSpeed;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(AttackActionMontage, AttackSpeedRate);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ARSCharacterBase::AttackActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackActionMontage);

	ComboTimerHandle_.Invalidate();
	SetComboCheckTimer();
}

void ARSCharacterBase::AttackActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensureMsgf(0 != CurrentCombo_, TEXT("Current Combo must be greater than 0."));

	CurrentCombo_ = 0;

	NotifyAttackActionEnd();
}

void ARSCharacterBase::NotifyAttackActionEnd()
{
	
}

void ARSCharacterBase::SetComboCheckTimer()
{
	int32 ComboIndex = CurrentCombo_ - 1;
	ensureMsgf(AttackActionData->EffectiveFrameCount.IsValidIndex(ComboIndex), TEXT("Invalid combo index : %d"), ComboIndex);

	const float AttackSpeedRate = Stat->GetCharacterStat().AttackSpeed;
	float ComboEffectiveTime = (AttackActionData->EffectiveFrameCount[ComboIndex] / AttackActionData->FrameRate) / AttackSpeedRate;
	if (0.0f < ComboEffectiveTime)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle_, this, &ARSCharacterBase::ComboCheck, ComboEffectiveTime, false);
	}
}

void ARSCharacterBase::ComboCheck()
{
	ComboTimerHandle_.Invalidate();
	if (HasNextAttackCommand_)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		CurrentCombo_ = (AttackActionData->MaxComboCount == CurrentCombo_) ? 1 : CurrentCombo_ + 1;
		FName NextSection = *FString::Printf(TEXT("%s%d"), *AttackActionData->MontageSectionNamePrefix, CurrentCombo_);
		AnimInstance->Montage_JumpToSection(NextSection, AttackActionMontage);
		SetComboCheckTimer();
		HasNextAttackCommand_ = false;
	}
}

void ARSCharacterBase::PlayAttackSound()
{
	ensureMsgf(AttackSound, TEXT("Attack sound is not assigned in %s"), *GetName());

	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		AttackSound,
		GetActorLocation(),
		1.0f,
		1.0f
	);
}

float ARSCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Stat->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void ARSCharacterBase::SetDead()
{
	IsDead_ = true;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
}

void ARSCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}


