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
#include "UI/RSWidgetComponent.h"
#include "UI/RSHpBarWidget.h"

// Sets default values
ARSCharacterBase::ARSCharacterBase()
{
	bUseControllerRotationPitch	= false;
	bUseControllerRotationYaw	= false;
	bUseControllerRotationRoll	= false;
	bReplicates = true;

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

	// Widget Component
	HpBar = CreateDefaultSubobject<URSWidgetComponent>(TEXT("Widget"));
	HpBar->SetIsReplicated(true);
	HpBar->SetWidgetSpace(EWidgetSpace::Screen);
	HpBar->SetDrawSize(FVector2D(100.f, 10.f));
	HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HpBar->SetHiddenInGame(true);
}

void ARSCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


void ARSCharacterBase::PostNetInit()
{
	Super::PostNetInit();
}

void ARSCharacterBase::ProcessAttackCombo()
{
	if (bIsDead)
		return;

	if (0 == CurrentCombo)
	{
		AttackActionBegin();
		return;
	}

	if (false == ComboTimerHandle.IsValid())
	{
		HasNextAttackCommand = false;
	}
	else
	{
		HasNextAttackCommand = true;
	}
}

void ARSCharacterBase::NotifyAttackAnimationToOtherClients()
{
	checkf(HasAuthority(), TEXT("NotifyAttackAnimationToOtherClients must be called only in server."));

	UWorld* World = GetWorld();
	if (!World)
		return;

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Iterator->Get();
		if (PC && PC->GetPawn())
		{
			if (ARSCharacterBase* Character = Cast<ARSCharacterBase>(PC->GetPawn()))
			{
				if (Character != this && !Character->IsLocallyControlled())
				{
					Character->ClientRPCPlayAttackAnimation(this);
				}
			}
		}
	}
}

void ARSCharacterBase::PlayAttackAnimationOnly()
{
	const float AttackSpeedRate = Stat->GetCharacterStat().AttackSpeed;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	AnimInstance->Montage_Play(AttackActionMontage, AttackSpeedRate);
}

void ARSCharacterBase::ClientRPCPlayAttackAnimation_Implementation(ARSCharacterBase* AttackingCharacter)
{
	ensureMsgf(AttackingCharacter, TEXT("AttackingCharacter is null in ARSCharacterBase::ClientRPCPlayAttackAnimation()"));

	if (AttackingCharacter)
	{
		AttackingCharacter->PlayAttackAnimationOnly();
	}
}

void ARSCharacterBase::MulticastRPCSetHpBarVisibility_Implementation(bool bVisible)
{
	if (HpBar)
	{
		HpBar->SetHiddenInGame(!bVisible);
	}
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

void ARSCharacterBase::AttackActionBegin()
{
	CurrentCombo = 1;

	const float AttackSpeedRate = Stat->GetCharacterStat().AttackSpeed;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(AttackActionMontage, AttackSpeedRate);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ARSCharacterBase::AttackActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackActionMontage);

	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();
}

void ARSCharacterBase::AttackActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensureMsgf(0 != CurrentCombo, TEXT("Current Combo must be greater than 0."));

	CurrentCombo = 0;

	NotifyAttackActionEnd();
}

void ARSCharacterBase::NotifyAttackActionEnd()
{
	
}

void ARSCharacterBase::SetComboCheckTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensureMsgf(AttackActionData->EffectiveFrameCount.IsValidIndex(ComboIndex), TEXT("Invalid combo index : %d"), ComboIndex);

	const float AttackSpeedRate = Stat->GetCharacterStat().AttackSpeed;
	float ComboEffectiveTime = (AttackActionData->EffectiveFrameCount[ComboIndex] / AttackActionData->FrameRate) / AttackSpeedRate;
	if (0.0f < ComboEffectiveTime)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &ARSCharacterBase::ComboCheck, ComboEffectiveTime, false);
	}
}

void ARSCharacterBase::ComboCheck()
{
	ComboTimerHandle.Invalidate();
	if (HasNextAttackCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		CurrentCombo = (AttackActionData->MaxComboCount == CurrentCombo) ? 1 : CurrentCombo + 1;
		FName NextSection = *FString::Printf(TEXT("%s%d"), *AttackActionData->MontageSectionNamePrefix, CurrentCombo);
		AnimInstance->Montage_JumpToSection(NextSection, AttackActionMontage);
		SetComboCheckTimer();
		HasNextAttackCommand = false;
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
	bIsDead = true;
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

void ARSCharacterBase::SetupHpBarComponent()
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

void ARSCharacterBase::SetupWidget(class UUserWidget* InUserWidget)
{
	URSHpBarWidget* HpBarWidget = Cast<URSHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		HpBarWidget->SetMaxHp(GetStatComponent()->GetCharacterStat().MaxHp);
		HpBarWidget->UpdateHpBar(GetStatComponent()->GetCurrentHp());
		GetStatComponent()->OnHpChanged.AddUObject(HpBarWidget, &URSHpBarWidget::UpdateHpBar);
	}
}