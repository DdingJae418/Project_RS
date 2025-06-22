// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/RSAnimInstance.h"
#include "RSPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSPlayerAnimInstance : public URSAnimInstance
{
	GENERATED_BODY()
	
public:
	URSPlayerAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Player)
	TObjectPtr<class ARSCharacterPlayer> PlayerOwner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player)
	uint8 bIsWeaponEquipped : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player)
	uint8 bIsAiming : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player)
	float AimingPitch;
};
