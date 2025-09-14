// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RSCharacterPlayer.h"
#include "Project_RS.h"
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
#include "Net/UnrealNetwork.h"
#include "Algo/MinElement.h"
#include "UI/RSWidgetComponent.h"


ARSCharacterPlayer::ARSCharacterPlayer()
	: CameraInterpSpeed{ 6.0f }
	, bIsWeaponEquipped {false}
	, bIsAiming{ false }
	, MaxAmmo{ 30 }
	, CurrentAmmo{ MaxAmmo }
	, CurrentMoney{ 0 }
	, CurrentCharacterControlType {ECharacterControlType::Shoulder}
	, bIsCameraTransitioning{ false }
	, TargetArmLength{ 300.0f }
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), *GunSpineSocketName);

	// Item Action
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ARSCharacterPlayer::PickUpAmmoItem)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ARSCharacterPlayer::PickUpMedicalItem)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &ARSCharacterPlayer::PickUpMoneyItem)));
}

void ARSCharacterPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetStatComponent()->SetCharacterStat(ECharacterName::Player);
	SetupHpBarComponent();
}

void ARSCharacterPlayer::PostNetInit()
{
	Super::PostNetInit();
}

void ARSCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
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

	if (bIsCameraTransitioning)
		UpdateCameraMovement(DeltaTime);
}

void ARSCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARSCharacterPlayer, bIsWeaponEquipped);
	DOREPLIFETIME(ARSCharacterPlayer, bIsAiming);
	DOREPLIFETIME_CONDITION(ARSCharacterPlayer, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARSCharacterPlayer, CurrentMoney, COND_OwnerOnly);
}

float ARSCharacterPlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority() && GetHpBarComponent()->bHiddenInGame && KINDA_SMALL_NUMBER < GetStatComponent()->GetCurrentHp())
	{
		MulticastRPCSetHpBarVisibility(true);
	}

	return DamageAmount;
}

void ARSCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
	EnhancedInputComponent->BindAction(TakeItemAction, ETriggerEvent::Triggered, this, &ARSCharacterPlayer::ServerRPCTakeItem);
}

void ARSCharacterPlayer::ChangeCharacterControl()
{
	if (ECharacterControlType::Shoulder == CurrentCharacterControlType)
	{
		SetCharacterControl(ECharacterControlType::Aiming);
		ShowAimingUI();
		if (HasAuthority())
		{
			bIsAiming = true;
			bIsWeaponEquipped = true; // Automatically equip weapon when aiming
		}
		else
		{
			ServerRPCSetAiming(true);
			ServerRPCSetWeaponEquipped(true);
		}
	}
	else if (ECharacterControlType::Aiming == CurrentCharacterControlType)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
		HideAimingUI();
		if (HasAuthority())
		{
			bIsAiming = false;
		}
		else
		{
			ServerRPCSetAiming(false);
		}
	}
}

void ARSCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	if (false == IsLocallyControlled())
	{
		return;
	}

	const URSCharacterControlData* NewCharacterControl = GetCharacterControlManager()[NewCharacterControlType];
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

	TargetArmLength				= CharacterControlData->TargetArmLength;
	FollowCameraTargetRotation	= CharacterControlData->RelativeRotation;
	FollowCameraTargetLocation	= CharacterControlData->RelativeLocation;

	CameraBoom->bUsePawnControlRotation		= CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch				= CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw					= CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll				= CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest			= CharacterControlData->bDoCollisionTest;

	bIsCameraTransitioning = true;
}

void ARSCharacterPlayer::SetupWeapon()
{
	if (true == bIsAiming) 
		return;

	bool bNewWeaponState = !bIsWeaponEquipped;
	if (HasAuthority())
	{
		bIsWeaponEquipped = bNewWeaponState;
	}
	else
	{
		ServerRPCSetWeaponEquipped(bNewWeaponState);
	}
}

void ARSCharacterPlayer::SetDead()
{
	Super::SetDead();

	if (HasAuthority() && false == IsLocallyControlled())
	{
		MulticastRPCSetHpBarVisibility(false);
	}

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
	const FName& SocketName = bIsWeaponEquipped ? *GunHandSocketName : *GunSpineSocketName;

	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void ARSCharacterPlayer::UpdateCameraMovement(float DeltaTIme)
{
	bool bFinished = true;

	float NewArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetArmLength, DeltaTIme, CameraInterpSpeed);
	if (false == FMath::IsNearlyEqual(NewArmLength, TargetArmLength, 0.1f))
		bFinished = false;
	CameraBoom->TargetArmLength = NewArmLength;

	FRotator NewRotation = FMath::RInterpTo(FollowCamera->GetRelativeRotation(), FollowCameraTargetRotation, DeltaTIme, CameraInterpSpeed);
	if (false == FollowCamera->GetRelativeRotation().Equals(FollowCameraTargetRotation, 0.1f))
		bFinished = false;
	FollowCamera->SetRelativeRotation(NewRotation);

	FVector NewLocation = FMath::VInterpTo(FollowCamera->GetRelativeLocation(), FollowCameraTargetLocation, DeltaTIme, CameraInterpSpeed);
	if (false == FollowCamera->GetRelativeLocation().Equals(FollowCameraTargetLocation, 0.1f))
		bFinished = false;
	FollowCamera->SetRelativeLocation(NewLocation);

	if (bFinished)
		bIsCameraTransitioning = false;
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
	if (CurrentAmmo == 0)
		return;

	if (IsLocallyControlled())
	{
		ProcessAttackCombo();
		ServerRPCFireRequest();
	}
}

void ARSCharacterPlayer::AttackHitCheck_Implementation()
{
	PlayAttackSound();

	if (IsLocallyControlled())
	{
		FHitResult OutHitResult;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

		const FVector Start = FollowCamera->GetComponentLocation();
		const FVector End = Start + FollowCamera->GetForwardVector() * GetStatComponent()->GetCharacterStat().AttackRange;

		bool HitDetected = GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, CCHANNEL_RSACTION, Params);
		if (HitDetected)
		{
			FHitReportData HitData(
				OutHitResult.GetActor(),
				OutHitResult.ImpactPoint,
				GetStatComponent()->GetCharacterStat().Attack,
				GetActorLocation()
			);

			ServerRPCReportHit(HitData);

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
}

void ARSCharacterPlayer::FindItem(ARSItem* InItem)
{
	checkf(InItem, TEXT("InItem is null in ARSCharacterPlayer::FindItem()"));
	ensureMsgf(false == CurrentItems.Contains(InItem), TEXT("Duplicate item is found in ARSCharacterPlayer::FindItem()"));
	checkf(HasAuthority(), TEXT("FindItem must be called only in server."));

	CurrentItems.Add(InItem);
}

void ARSCharacterPlayer::LoseItem(ARSItem* InItem)
{
	checkf(InItem, TEXT("InItem is null in ARSCharacterPlayer::FindItem()"));
	ensureMsgf(CurrentItems.Contains(InItem), TEXT("InItem is not found in ARSCharacterPlayer::LoseItem()"));
	checkf(HasAuthority(), TEXT("LoseItem must be called only in server."));

	CurrentItems.Remove(InItem);
}

void ARSCharacterPlayer::SetupWidget(UUserWidget* InUserWidget)
{
	Super::SetupWidget(InUserWidget);

	if (URSHUDWidget* InHUDWidget = Cast<URSHUDWidget>(InUserWidget))
	{
		InHUDWidget->SetMaxHp(GetStatComponent()->GetCharacterStat().MaxHp);
		InHUDWidget->UpdateHpBar(GetStatComponent()->GetCurrentHp());
		InHUDWidget->SetMaxAmmo(MaxAmmo);
		InHUDWidget->UpdateOwningAmmo(CurrentAmmo);
		InHUDWidget->UpdateOwningMoney(CurrentMoney);

		GetStatComponent()->OnHpChanged.AddUObject(InHUDWidget, &URSHUDWidget::UpdateHpBar);
		OnOwningAmmonChanged.AddUObject(InHUDWidget, &URSHUDWidget::UpdateOwningAmmo);
		OnOwningMoneyChanged.AddUObject(InHUDWidget, &URSHUDWidget::UpdateOwningMoney);
	}
}

void ARSCharacterPlayer::ServerRPCTakeItem_Implementation()
{
	if (CurrentItems.IsEmpty())
		return;

	TArray<ARSItem*> ValidItems = CurrentItems.FilterByPredicate([](const ARSItem* Item) { return ::IsValid(Item); });
	if (ValidItems.IsEmpty())
		return;

	ARSItem* item = *Algo::MinElementBy(ValidItems, [this](ARSItem* Item) { return Item->GetDistanceTo(this); });

	URSItemData* itemData = item->GetItemData();
	TakeItemActions[(uint8)itemData->GetType()].ItemDelegate.ExecuteIfBound(itemData);
	item->ConsumeItem(); // Automatically call LoseItem() after consuming
}

void ARSCharacterPlayer::PickUpAmmoItem(URSItemData* InItemData)
{
	checkf(InItemData, TEXT("InItemData is null in ARSCharacterPlayer::PickUpAmmoItem()"));
	checkf(HasAuthority(), TEXT("PickUpAmmoItem must be called only in server."));

	if (URSAmmoItemData* AmmoItemData = Cast<URSAmmoItemData>(InItemData))
	{
		CurrentAmmo = FMath::Clamp<uint8>(CurrentAmmo + AmmoItemData->GetAmmoAmount(), 0, MaxAmmo);
		OnRep_CurrentAmmo();
	}
}

void ARSCharacterPlayer::PickUpMedicalItem(URSItemData* InItemData)
{
	checkf(InItemData, TEXT("InItemData is null in ARSCharacterPlayer::PickUpMedicalItem()"));
	checkf(HasAuthority(), TEXT("PickUpMedicalItem must be called only in server."));

	if (URSMedicalItemData* MedicalItemData = Cast<URSMedicalItemData>(InItemData))
	{
		GetStatComponent()->RestoreHp(MedicalItemData->GetRestoreAmount());
	}
}

void ARSCharacterPlayer::PickUpMoneyItem(URSItemData* InItemData)
{
	checkf(InItemData, TEXT("InItemData is null in ARSCharacterPlayer::PickUpMoneyItem()"));
	checkf(HasAuthority(), TEXT("PickUpMoneyItem must be called only in server."));

	if (URSMoneyItemData* MoneyItemData = Cast<URSMoneyItemData>(InItemData))
	{
		CurrentMoney += MoneyItemData->GetMoneyValue();
		OnRep_CurrentMoney();
	}
}

void ARSCharacterPlayer::ShowAimingUI()
{
	if (false == IsLocallyControlled())
		return;
		
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
	if (false == IsLocallyControlled())
		return;
		
	checkf(AimingPointWidget, TEXT("AimingPointWidget is not set in ARSCharacterPlayer::HideAimingUI()"));
	ensureMsgf(AimingPointWidget->IsInViewport(), TEXT("AimingPointWidget is not in viewport in ARSCharacterPlayer::HideAimingUI()"));

	AimingPointWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ARSCharacterPlayer::ServerRPCSetWeaponEquipped_Implementation(bool bEquipped)
{
	bIsWeaponEquipped = bEquipped;
}

void ARSCharacterPlayer::ServerRPCSetAiming_Implementation(bool bNewAiming)
{
	bIsAiming = bNewAiming;
	bUseControllerRotationYaw = bNewAiming;
}

bool ARSCharacterPlayer::ServerRPCFireRequest_Validate()
{
	return CurrentAmmo > 0 && !IsDead();
}

void ARSCharacterPlayer::ServerRPCFireRequest_Implementation()
{
	CurrentAmmo--;
	OnRep_CurrentAmmo();

	PlayAttackAnimationOnly();
	NotifyAttackAnimationToOtherClients();
}

bool ARSCharacterPlayer::ServerRPCReportHit_Validate(const FHitReportData& HitData)
{
	if (false == ::IsValid(HitData.HitTarget) || HitData.Damage <= 0.0f)
		return false;

	// Validate attack range
	float DistanceToTarget			= FVector::Dist(GetActorLocation(), HitData.HitTarget->GetActorLocation());
	float MaxAttackRange			= GetStatComponent()->GetCharacterStat().AttackRange;
	float MaxAllowedAttackDistance	= MaxAttackRange * 1.1f;

	if (MaxAllowedAttackDistance < DistanceToTarget)
		return false;

	// Validate hit position vs actual target position
	FVector CurrentTargetLocation			= HitData.HitTarget->GetActorLocation();
	float HitPositionDiscrepancy			= FVector::Dist(HitData.HitLocation, CurrentTargetLocation);
	float MaxAllowedHitPositionDiscrepancy	= 200.0f; // 2 meters tolerance

	if (MaxAllowedHitPositionDiscrepancy < HitPositionDiscrepancy)
		return false;

	return true;
}

void ARSCharacterPlayer::ServerRPCReportHit_Implementation(const FHitReportData& HitData)
{
	FDamageEvent DamageEvent;
	HitData.HitTarget->TakeDamage(HitData.Damage, DamageEvent, GetController(), this);
}


void ARSCharacterPlayer::OnRep_CurrentAmmo()
{
	OnOwningAmmonChanged.Broadcast(CurrentAmmo);
}

void ARSCharacterPlayer::OnRep_CurrentMoney()
{
	OnOwningMoneyChanged.Broadcast(CurrentMoney);
}

