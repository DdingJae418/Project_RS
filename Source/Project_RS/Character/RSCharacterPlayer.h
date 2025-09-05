// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RSCharacterBase.h"
#include "Interface/RSAnimationAttackInterface.h"
#include "Interface/RSCharacterItemInterface.h"
#include "Interface/RSWidgetInterface.h"
#include "RSCharacterPlayer.generated.h"

class UInputAction;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class URSCharacterControlData;
class URSItemData;
class ARSItem;
struct FInputActionValue;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHitTargetDelegate, AActor* /*HitTarget*/, const FHitResult&);
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

const FString GunSpineSocketName	= TEXT("gun_spine");
const FString GunHandSocketName		= TEXT("gun_hand");


UCLASS(BlueprintType, Blueprintable)
class PROJECT_RS_API ARSCharacterPlayer : public ARSCharacterBase, public IRSAnimationAttackInterface, public IRSCharacterItemInterface, public IRSWidgetInterface
{
	GENERATED_BODY()

public:
	ARSCharacterPlayer();
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	bool IsWeaponEquipped() const { return bIsWeaponEquipped; }
	bool IsAiming() const { return bIsAiming; }
	void AttachWeapon();
	float GetAimingPitch() const;

	uint8 GetCurrentAmmo() const { return CurrentAmmo; }
	uint8 GetCurrentMoney() const { return CurrentMoney; }

	FOnHitTargetDelegate OnHitTarget;
	FOnOwningMoneyChangeDelegate OnOwningMoneyChanged;

	//~ Start IRSCharacterItemInterface interface
	virtual void FindItem(ARSItem* InItem) override;
	virtual void LoseItem(ARSItem* InItem) override;
	//~ End IRSCharacterItenInterface interface

	//~ Start of IRSWidgetInterface interface
	virtual void SetupWidget(class UUserWidget* InUserWidget) override;
	//~ End of IRSWidgetInterface interface

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraInterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UUserWidget> AimingPointWidget;

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

	void UpdateCameraMovement(float DeltaTime);

	void SetupWeapon();
	virtual void SetDead() override;

	void ChangeCharacterControl();
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	virtual void SetCharacterControlData(const URSCharacterControlData* ControlData) override;

	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);
	void AimingMove(const FInputActionValue& Value);
	void AimingLook(const FInputActionValue& Value);
	void Fire();

	//~ Start AActor interface
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
	//~ End AActor interface

	//~ Start IRSAnimationAttackInterface interface
	virtual void AttackHitCheck_Implementation() override;
	//~ End IRSAnimtationAttackInterface interface

private:
	UFUNCTION(Server, Reliable)
	void ServerRPCTakeItem();	

	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions;

	void PickUpAmmoItem(URSItemData* InItemData);
	void PickUpMedicalItem(URSItemData* InItemData);
	void PickUpMoneyItem(URSItemData* InItemData);

	void SetCurrentAmmo(uint8 NewAmmo) { CurrentAmmo = FMath::Clamp<uint8>(NewAmmo, 0, MaxAmmo); OnOwningAmmonChanged.Broadcast(CurrentAmmo); }
	void SetCurrentMoney(uint8 NewMoney) { CurrentMoney = NewMoney; OnOwningMoneyChanged.Broadcast(CurrentMoney); }

	void ShowAimingUI();
	void HideAimingUI();

	FOnOwningAmmoChangeDelegate OnOwningAmmonChanged;

	UPROPERTY()
	TArray<ARSItem*> CurrentItems;

	ECharacterControlType CurrentCharacterControlType;
	bool bIsWeaponEquipped;
	bool bIsAiming;

	FRotator FollowCameraTargetRotation;
	FVector	FollowCameraTargetLocation;
	bool bIsCameraTransitioning;
	float TargetArmLength;

	const uint8 MaxAmmo = 60;
	uint8 CurrentAmmo	= MaxAmmo;
	uint32 CurrentMoney = 0;
};
