// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_SetupWeapon.h"
#include "Character/RSCharacterPlayer.h"

void UAnimNotify_SetupWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	ARSCharacterPlayer* playerCharacter = Cast<ARSCharacterPlayer>(MeshComp->GetOwner());
	if (!playerCharacter)
	{
		return;
	}

	playerCharacter->AttachWeapon();
}
