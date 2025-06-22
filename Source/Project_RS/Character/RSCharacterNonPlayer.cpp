// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RSCharacterNonPlayer.h"

ARSCharacterNonPlayer::ARSCharacterNonPlayer()
{

}

void ARSCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), DeadEventDelayTime, false);
}
