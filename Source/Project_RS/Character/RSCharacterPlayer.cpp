// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RSCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "RSCharacterControlData.h"
#include "RSAttackActionData.h"
#include "Physics/RSCollision.h"
#include "Engine/DamageEvents.h"
#include "Item/RSItemData.h"
#include "Item/RSItems.h"
#include "CharacterStat/RSCharacterStatComponent.h"
#include "Enums/ECharacterName.h"
#include "UI/RSHUDWidget.h"
#include "Interface/RSGameInterface.h"
#include "GameFramework/GameModeBase.h"
#include "Player/RSPlayerController.h"

ARSCharacterPlayer::ARSCharacterPlayer()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), *GunSpineSocketName);

	CurrentCharacterControlType		= ECharacterControlType::Shoulder;
	bIsWeaponEquipped_				= false;
	bIsAiming_						= false;
	bIsCameraTransitioning_			= false;
	CameraInterpSpeed				= 6.0f;
	TargetArmLength_				= 300.0f;

	// Item Action
	TakeItemActions_.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ARSCharacterPlayer::PickUpAmmoItem)));
	TakeItemActions_.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ARSCharacterPlayer::PickUpMedicalItem)));
	TakeItemActions_.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ARSCharacterPlayer::PickUpMoneyItem)));
	TakeItemActions_.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ARSCharacterPlayer::PickUpStoryItem)));
}

void ARSCharacterPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->SetCharacterStat(ECharacterName::Player);
}

void ARSCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterControl(ECharacterControlType::Shoulder);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}
}

void ARSCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsCameraTransitioning_)
		UpdateCameraMovement(DeltaTime);
}

void ARSCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Started, this, &ARSCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Completed, this, &ARSCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(SetupWeaponAction, ETriggerEvent::Triggered, this, &ARSCharacterPlayer::SetupWeapon);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &ARSCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &ARSCharacterPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(AimingMoveAction, ETriggerEvent::Triggered, this, &ARSCharacterPlayer::AimingMove);
	EnhancedInputComponent->BindAction(AimingLookAction, ETriggerEvent::Triggered, this, &ARSCharacterPlayer::AimingLook);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ARSCharacterPlayer::Fire);
	EnhancedInputComponent->BindAction(TakeItemAction, ETriggerEvent::Triggered, this, &ARSCharacterPlayer::TakeItem);

}

void ARSCharacterPlayer::ChangeCharacterControl()
{
	if (ECharacterControlType::Shoulder == CurrentCharacterControlType)
	{
		SetCharacterControl(ECharacterControlType::Aiming);
		ShowAimingUI();
		bIsAiming_			= true;
		bIsWeaponEquipped_	= true; // Automatically equip weapon when aiming
	}
	else if (ECharacterControlType::Aiming == CurrentCharacterControlType)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
		HideAimingUI();
		bIsAiming_ = false;
	}
}

void ARSCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	URSCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	if (APlayerController* PlayerController = CastChecked<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}
	
	CurrentCharacterControlType = NewCharacterControlType;
}

void ARSCharacterPlayer::SetCharacterControlData(const class URSCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	TargetArmLength_			= CharacterControlData->TargetArmLength;
	FollowCameraTargetRotation_	= CharacterControlData->RelativeRotation;
	FollowCameraTargetLocation_	= CharacterControlData->RelativeLocation;

	CameraBoom->bUsePawnControlRotation		= CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch				= CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw					= CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll				= CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest			= CharacterControlData->bDoCollisionTest;

	bIsCameraTransitioning_ = true;
}

void ARSCharacterPlayer::SetupWeapon()
{
	if (true == bIsAiming_) return;

	bIsWeaponEquipped_ = !bIsWeaponEquipped_;
}

void ARSCharacterPlayer::SetDead()
{
	Super::SetDead();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		DisableInput(PlayerController);

		IRSGameInterface* RSGameMode = Cast<IRSGameInterface>(GetWorld()->GetAuthGameMode());
		if (RSGameMode)
		{
			RSGameMode->OnPlayerDead();
		}
	}
}

float ARSCharacterPlayer::GetAimingPitch() const
{
	float AimingPitch = FRotator::NormalizeAxis(GetControlRotation().Pitch);
	AimingPitch = FMath::Clamp(AimingPitch, -45, 45);

	return AimingPitch;
}

void ARSCharacterPlayer::AttachWeapon()
{
	const FName& SocketName = bIsWeaponEquipped_ ? *GunHandSocketName : *GunSpineSocketName;

	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void ARSCharacterPlayer::UpdateCameraMovement(float DeltaTIme)
{
	bool bFinished = true;

	float NewArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetArmLength_, DeltaTIme, CameraInterpSpeed);
	if (false == FMath::IsNearlyEqual(NewArmLength, TargetArmLength_, 0.1f))
		bFinished = false;
	CameraBoom->TargetArmLength = NewArmLength;

	FRotator NewRotation = FMath::RInterpTo(FollowCamera->GetRelativeRotation(), FollowCameraTargetRotation_, DeltaTIme, CameraInterpSpeed);
	if (false == FollowCamera->GetRelativeRotation().Equals(FollowCameraTargetRotation_, 0.1f))
		bFinished = false;
	FollowCamera->SetRelativeRotation(NewRotation);

	FVector NewLocation = FMath::VInterpTo(FollowCamera->GetRelativeLocation(), FollowCameraTargetLocation_, DeltaTIme, CameraInterpSpeed);
	if (false == FollowCamera->GetRelativeLocation().Equals(FollowCameraTargetLocation_, 0.1f))
		bFinished = false;
	FollowCamera->SetRelativeLocation(NewLocation);

	if (bFinished)
		bIsCameraTransitioning_ = false;
}

void ARSCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ARSCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ARSCharacterPlayer::AimingMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), MovementVector.X);
	AddMovementInput(GetActorRightVector(), MovementVector.Y);
}

void ARSCharacterPlayer::AimingLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ARSCharacterPlayer::Fire()
{
	if (CurrentAmmo_ == 0)
		return;

	SetCurrentAmmo(CurrentAmmo_ - 1);

	ProcessAttackCommand();
}

void ARSCharacterPlayer::AttackHitCheck_Implementation()
{
	PlayAttackSound();

	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const FVector Start = FollowCamera->GetComponentLocation();
	const FVector End = Start + FollowCamera->GetForwardVector() * Stat->GetCharacterStat().AttackRange;
	
	bool HitDetected = GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, CCHANNEL_RSACTION, Params);
	if (HitDetected)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(Stat->GetCharacterStat().Attack, DamageEvent, GetController(), this);

		OnHitTarget.Broadcast(OutHitResult.GetActor(), OutHitResult);
	}

#if ENABLE_DRAW_DEBUG

	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), Start, End, DrawColor, false, 5.0f, 0, 0.5f);
	if (HitDetected)
	{
		DrawDebugPoint(GetWorld(), OutHitResult.ImpactPoint, 15.0f, FColor::Yellow, false, 5.0f);
	}
#endif
}

void ARSCharacterPlayer::FindItem(class ARSItem* InItem)
{
	ensureMsgf(InItem, TEXT("InItem is null in ARSCharacterPlayer::FindItem()"));
	ensureMsgf(false == CurrentItems_.Contains(InItem), TEXT("Duplicate item is found in ARSCharacterPlayer::FindItem()"));

	CurrentItems_.Add(InItem);
}

void ARSCharacterPlayer::LoseItem(class ARSItem* InItem)
{
	ensureMsgf(InItem, TEXT("InItem is null in ARSCharacterPlayer::FindItem()"));
	ensureMsgf(CurrentItems_.Contains(InItem), TEXT("InItem is not found in ARSCharacterPlayer::LoseItem()"));

	CurrentItems_.Remove(InItem);
}

void ARSCharacterPlayer::SetupWidget(class UUserWidget* InUserWidget)
{

	if (URSHUDWidget* InHUDWidget = Cast<URSHUDWidget>(InUserWidget))
	{
		InHUDWidget->SetMaxHp(Stat->GetCharacterStat().MaxHp);
		InHUDWidget->UpdateHpBar(Stat->GetCurrentHp());
		InHUDWidget->SetMaxAmmo(MaxAmmo_);
		InHUDWidget->UpdateOwningAmmo(CurrentAmmo_);
		InHUDWidget->UpdateOwningMoney(CurrentMoney_);

		Stat->OnHpChanged.AddUObject(InHUDWidget, &URSHUDWidget::UpdateHpBar);
		OnOwningAmmonChanged.AddUObject(InHUDWidget, &URSHUDWidget::UpdateOwningAmmo);
		OnOwningMoneyChanged.AddUObject(InHUDWidget, &URSHUDWidget::UpdateOwningMoney);
	}
}

void ARSCharacterPlayer::TakeItem()
{
	if (CurrentItems_.Num() == 0)
		return;

	ARSItem* item = CurrentItems_[0];
	URSItemData* itemData = item->GetItemData();

	TakeItemActions_[(uint8)itemData->GetType()].ItemDelegate.ExecuteIfBound(itemData);
	item->ConsumeItem(); // Automatically call LoseItem() after consuming
}

void ARSCharacterPlayer::PickUpAmmoItem(class URSItemData* InItemData)
{
	if (URSAmmoItemData* AmmoItemData = Cast<URSAmmoItemData>(InItemData))
	{
		SetCurrentAmmo(CurrentAmmo_ + AmmoItemData->GetAmmoAmount());
	}
}

void ARSCharacterPlayer::PickUpMedicalItem(class URSItemData* InItemData)
{
	if (URSMedicalItemData* MedicalItemData = Cast<URSMedicalItemData>(InItemData))
	{
		Stat->RestoreHp(MedicalItemData->GetRestoreAmount());
	}
}

void ARSCharacterPlayer::PickUpMoneyItem(class URSItemData* InItemData)
{
	if (URSMoneyItemData* MoneyItemData = Cast<URSMoneyItemData>(InItemData))
	{
		SetCurrentMoney(CurrentMoney_ + MoneyItemData->GetMoneyValue());
	}
}

void ARSCharacterPlayer::PickUpStoryItem(class URSItemData* InItemData)
{

}

void ARSCharacterPlayer::ShowAimingUI()
{
	checkf(AimingPointWidget, TEXT("AimingPointWidget is not set in ARSCharacterPlayer::ShowAimingUI()"));

	if (false == AimingPointWidget->IsInViewport())
	{
		AimingPointWidget->AddToViewport();
	}
	else
	{
		AimingPointWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ARSCharacterPlayer::HideAimingUI()
{
	checkf(AimingPointWidget, TEXT("AimingPointWidget is not set in ARSCharacterPlayer::HideAimingUI()"));
	ensureMsgf(AimingPointWidget->IsInViewport(), TEXT("AimingPointWidget is not in viewport in ARSCharacterPlayer::HideAimingUI()"));

	AimingPointWidget->SetVisibility(ESlateVisibility::Hidden);
}

