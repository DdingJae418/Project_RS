// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RSCharacterBase.h"
#include "EnhancedInputComponent.h"
#include "Interface/RSAnimationAttackInterface.h"
#include "Interface/RSCharacterItemInterface.h"
#include "RSCharacterPlayer.generated.h"


DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class URSItemData* /*InItemData*/);
USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()
	FTakeItemDelegateWrapper() {}
	FTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate) : ItemDelegate(InItemDelegate) {}
	FOnTakeItemDelegate ItemDelegate;
};

/**
 * 
 */
UCLASS()
class PROJECT_RS_API ARSCharacterPlayer : public ARSCharacterBase, public IRSAnimationAttackInterface, public IRSCharacterItemInterface
{
	GENERATED_BODY()
	
public:
	ARSCharacterPlayer();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
// Character Control Section

public:
	float GetAimingPitch() const;
	bool IsWeaponEquipped() const { return bIsWeaponEquipped_; }
	bool IsAiming() const { return bIsAiming_; }
	void AttachWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Weapon;

	void ChangeCharacterControl();
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	virtual void SetCharacterControlData(const class URSCharacterControlData* ControlData) override;
	void SetupWeapon();

private:
	bool bIsWeaponEquipped_;
	bool bIsAiming_;


// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraInterpSpeed;

private:
	bool		bIsCameraTransitioning_;
	float		TargetArmLength_;
	FRotator	FollowCameraTargetRotation_;
	FVector		FollowCameraTargetLocation_;

	void UpdateCameraMovement(float DeltaTime);


// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeControlAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimingMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimingLookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SetupWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> TakeItemAction;


	ECharacterControlType CurrentCharacterControlType;


	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);

	void AimingMove(const FInputActionValue& Value);
	void AimingLook(const FInputActionValue& Value);

	void Fire();

	virtual void AttackHitCheck() override;

// Item Interface Section
public:

	virtual void FindItem(class ARSItem* InItem) override;
	virtual void LoseItem(class ARSItem* InItem) override;
	void TakeItem();
	void PickUpAmmoItem(class URSItemData* InItemData);
	void PickUpMedicalItem(class URSItemData* InItemData);
	void PickUpMoneyItem(class URSItemData* InItemData);
	void PickUpStoryItem(class URSItemData* InItemData);

private:
	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions_;

	UPROPERTY()
	TArray<TObjectPtr<class ARSItem>> CurrentItems_; // The items that the player is currently interacting with
};
