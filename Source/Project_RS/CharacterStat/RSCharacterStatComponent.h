// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/RSCharacterStat.h"
#include "Enums/ECharacterName.h"
#include "RSCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_RS_API URSCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URSCharacterStatComponent();


public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

	void SetCharacterStat(ECharacterName InCharacterName);
	FORCEINLINE FRSCharacterStat GetCharacterStat() const { return CharacterStat; }
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }
	float ApplyDamage(float InDamage);
	void RestoreHp(float InHp) { SetHp(CurrentHp + InHp); }

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	FRSCharacterStat CharacterStat;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;
		
	void SetHp(float NewHp);
};
