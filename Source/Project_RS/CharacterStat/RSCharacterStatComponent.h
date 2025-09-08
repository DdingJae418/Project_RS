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
	URSCharacterStatComponent();

	FORCEINLINE FRSCharacterStat GetCharacterStat() const { return CharacterStat; }
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }

	void SetCharacterStat(ECharacterName InCharacterName);
	float ApplyDamage(float InDamage);
	void RestoreHp(float InHp) { SetHp(CurrentHp + InHp); }

	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

	//~ Start UActorComponent interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UActorComponent interface

private:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	FRSCharacterStat CharacterStat;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHp, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	float CurrentHp;

	UFUNCTION()
	void OnRep_CurrentHp() { SetHp(CurrentHp); }
		
	void SetHp(float NewHp);
};
