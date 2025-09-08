// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/RSCharacterStatComponent.h"
#include "GameData/RSGameSingleton.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
URSCharacterStatComponent::URSCharacterStatComponent()
{
	bWantsInitializeComponent = true;
	SetIsReplicated(true);
}

void URSCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URSCharacterStatComponent, CurrentHp);
}

void URSCharacterStatComponent::SetCharacterStat(ECharacterName InCharacterName)
{
	CharacterStat = URSGameSingleton::Get().GetCharacterStat(InCharacterName);

	SetHp(CharacterStat.MaxHp);
}

float URSCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0.0f, InDamage);
	
	SetHp(PrevHp - ActualDamage);

	return ActualDamage;
}

void URSCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, CharacterStat.MaxHp);

	OnHpChanged.Broadcast(CurrentHp);

	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}
}

