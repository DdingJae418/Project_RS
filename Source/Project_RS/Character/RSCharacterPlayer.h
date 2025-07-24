// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RSCharacterBase.h"
#include "Interface/RSAnimationAttackInterface.h"
#include "Interface/RSCharacterItemInterface.h"
#include "RSCharacterPlayer.generated.h"

// Forward Declarations
class UInputAction;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class URSCharacterControlData;
class URSItemData;
class ARSItem;
struct FInputActionValue;

DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, URSItemData* /*InItemData*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOwningAmmoChangeDelegate, uint8 /*NewAmmo*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOwningMoneyChangeDelegate, uint32 /*NewMoney*/);

USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()
	
	FTakeItemDelegateWrapper() {}
	FTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate) : ItemDelegate(InItemDelegate) {}
	
	FOnTakeItemDelegate ItemDelegate;
};

/**
 * Player-controlled character class.
 * Extends base character with input handling, camera control, weapon systems, and item interaction.
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECT_RS_API ARSCharacterPlayer : public ARSCharacterBase, public IRSAnimationAttackInterface, public IRSCharacterItemInterface
{
	GENERATED_BODY()

// ================================================================================================
// PUBLIC INTERFACE
// ================================================================================================
public:
	// Constructor & Core Overrides
	ARSCharacterPlayer();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Character Control Interface
	float GetAimingPitch() const;

	bool IsWeaponEquipped() const { return bIsWeaponEquipped_; }
	
	bool IsAiming() const { return bIsAiming_; }
	
	void AttachWeapon();

	// Item Interface
	uint8 GetCurrentAmmo() const { return CurrentAmmo_; }
	uint8 GetCurrentMoney() const { return CurrentMoney_; }

	// IRSCharacterItemInterface Implementation
	virtual void FindItem(ARSItem* InItem) override;
	virtual void LoseItem(ARSItem* InItem) override;

	// UI Interface
	virtual void SetupWidget(class UUserWidget* InUserWidget) override;


// ================================================================================================
// PROTECTED IMPLEMENTATION
// ================================================================================================
protected:
	// Core Overrides
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	// ========== Camera System ==========
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraInterpSpeed;

	void UpdateCameraMovement(float DeltaTime);

	// ========== Weapon System ==========
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Weapon;

	void SetupWeapon();

	// ========== Character Control System ==========
	void ChangeCharacterControl();
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	virtual void SetCharacterControlData(const URSCharacterControlData* ControlData) override;

	// ========== Input System ==========
	// Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ChangeControlAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ShoulderMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ShoulderLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AimingMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AimingLookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SetupWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TakeItemAction;

	// Input Handlers
	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);
	void AimingMove(const FInputActionValue& Value);
	void AimingLook(const FInputActionValue& Value);
	void Fire();

	// IRSAnimationAttackInterface Implementation
	virtual void AttackHitCheck_Implementation() override;


// ================================================================================================
// PRIVATE IMPLEMENTATION
// ================================================================================================
private:
	// ========== Character Control Internal State ==========
	ECharacterControlType CurrentCharacterControlType;
	bool bIsWeaponEquipped_;
	bool bIsAiming_;

	// ========== Camera Internal State ==========
	bool bIsCameraTransitioning_;
	float TargetArmLength_;
	FRotator FollowCameraTargetRotation_;
	FVector	FollowCameraTargetLocation_;

	// ========== Item System ==========
	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions_;

	UPROPERTY()
	TArray<TObjectPtr<ARSItem>> CurrentItems_;

	void TakeItem();	
	void PickUpAmmoItem(URSItemData* InItemData);
	void PickUpMedicalItem(URSItemData* InItemData);
	void PickUpMoneyItem(URSItemData* InItemData);
	void PickUpStoryItem(URSItemData* InItemData);

	void SetCurrentAmmo(uint8 NewAmmo) { CurrentAmmo_ = FMath::Clamp<uint8>(NewAmmo, 0, MaxAmmo_); OnOwningAmmonChanged.Broadcast(CurrentAmmo_); }
	void SetCurrentMoney(uint8 NewMoney) { CurrentMoney_ = NewMoney; OnOwningMoneyChanged.Broadcast(CurrentMoney_); }

	FOnOwningAmmoChangeDelegate OnOwningAmmonChanged;
	FOnOwningMoneyChangeDelegate OnOwningMoneyChanged;

	const uint8 MaxAmmo_ = 30;
	uint8 CurrentAmmo_ = MaxAmmo_;
	uint32 CurrentMoney_ = 0;
};
