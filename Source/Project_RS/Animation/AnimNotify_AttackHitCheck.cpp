// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AttackHitCheck.h"
#include "Interface/RSAnimationAttackInterface.h"
#include "Project_RS.h"
#include "GameFramework/Character.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (UObject* Owner = MeshComp ? MeshComp->GetOwner() : nullptr)
	{
		if (Owner->GetClass()->ImplementsInterface(URSAnimationAttackInterface::StaticClass()))
		{
			IRSAnimationAttackInterface::Execute_AttackHitCheck(Owner);
		}
	}
}
