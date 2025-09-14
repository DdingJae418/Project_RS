// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RSCharacterBase.h"
#include "Interface/RSAnimationAttackInterface.h"
#include "Interface/RSCharacterItemInterface.h"
#include "Interface/RSWidgetInterface.h"
#include "RSCharacterPlayer.generated.h"

class ARSItem;
class UInputAction;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class URSCharacterControlData;
class URSItemData;
struct FInputActionValue;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHitTargetDelegate, AActor* /*HitTarget*/, const FHitResult&);
DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, URSItemData* /*InItemData*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOwningAmmoChangeDelegate, uint8 /*NewAmmo*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOwningMoneyChangeDelegate, uint32 /*NewMoney*/);

USTRUCT()
struct FHitReportData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> HitTarget;

	UPROPERTY()
	FVector HitLocation;

	UPROPERTY()
	float Damage;

	UPROPERTY()
	FVector AttackerLocation;

	FHitReportData()
		: HitTarget(nullptr)
		, HitLocation(FVector::ZeroVector)
		, Damage(0.0f)
		, AttackerLocation(FVector::ZeroVector)
	{}

	FHitReportData(AActor* InHitTarget, const FVector& InHitLocation, float InDamage, const FVector& InAttackerLocation)
		: HitTarget(InHitTarget)
		, HitLocation(InHitLocation)
		, Damage(InDamage)
		, AttackerLocation(InAttackerLocation)
	{}
};

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
class PROJECT_RS_API ARSCharacterPlayer : public ARSCharacterBase, public IRSAnimationAttackInterface, public IRSCharacterItemInterface
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

	//~ Start AActor interface
	virtual void PostNetInit() override;
	//~ End AActor interface

	//~ Start IRSCharacterItemInterface interface
	virtual void FindItem(ARSItem* InItem) override;
	virtual void LoseItem(ARSItem* InItem) override;
	//~ End IRSCharacterItenInterface interface

	//~ Start of IRSWidgetInterface interface
	virtual void SetupWidget(class UUserWidget* InUserWidget) override;
	//~ End of IRSWidgetInterface interface

protected:

	//~ Start AActor interface
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End AActor interface

	//~ Start ARSCharacterBase interface
	virtual void SetCharacterControlData(const URSCharacterControlData* ControlData) override;
	virtual void SetDead() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	//~ End ARSCharacterBase interface

	//~ Start IRSAnimationAttackInterface interface
	virtual void AttackHitCheck_Implementation() override;
	//~ End IRSAnimtationAttackInterface interface

private:
	UFUNCTION(Server, Reliable)
	void ServerRPCTakeItem();

	UFUNCTION(Server, Reliable)
	void ServerRPCSetWeaponEquipped(bool bEquipped);

	UFUNCTION(Server, Reliable)
	void ServerRPCSetAiming(bool bNewAiming);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCFireRequest();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCReportHit(const FHitReportData& HitData);

	UFUNCTION()
	void OnRep_CurrentAmmo();

	UFUNCTION()
	void OnRep_CurrentMoney();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	float CameraInterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UUserWidget> AimingPointWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ChangeControlAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ShoulderMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ShoulderLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AimingMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AimingLookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SetupWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TakeItemAction;

	UPROPERTY(Replicated)
	uint8 bIsWeaponEquipped : 1;
	
	UPROPERTY(Replicated)
	uint8 bIsAiming : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	uint8 MaxAmmo;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo, VisibleInstanceOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	uint8 CurrentAmmo;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentMoney, VisibleInstanceOnly, Category = "Item", Meta = (AllowPrivateAccess = "true"))
	uint32 CurrentMoney;

	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions;

	void UpdateCameraMovement(float DeltaTime);

	void ChangeCharacterControl();
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);

	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);
	void AimingMove(const FInputActionValue& Value);
	void AimingLook(const FInputActionValue& Value);
	void Fire();

	void SetupWeapon();

	void PickUpAmmoItem(URSItemData* InItemData);
	void PickUpMedicalItem(URSItemData* InItemData);
	void PickUpMoneyItem(URSItemData* InItemData);

	void ShowAimingUI();
	void HideAimingUI();

	FOnOwningAmmoChangeDelegate OnOwningAmmonChanged;

	TArray<ARSItem*> CurrentItems;
	ECharacterControlType CurrentCharacterControlType;
	FRotator FollowCameraTargetRotation;
	FVector	FollowCameraTargetLocation;

	bool bIsCameraTransitioning;
	float TargetArmLength;
};
