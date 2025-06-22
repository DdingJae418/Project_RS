// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/RSPlayerAnimInstance.h"
#include "Character/RSCharacterPlayer.h"

URSPlayerAnimInstance::URSPlayerAnimInstance()
{
	bIsWeaponEquipped = false;
}

void URSPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerOwner = Cast<ARSCharacterPlayer>(GetOwningActor());
}

void URSPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (PlayerOwner)
	{
		bIsWeaponEquipped	= PlayerOwner->IsWeaponEquipped();
		bIsAiming			= PlayerOwner->IsAiming();
		AimingPitch			= PlayerOwner->GetAimingPitch();
	}
}
