// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/RSWidgetInterface.h"
#include "RSCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8 
{
	Shoulder,
	Aiming
};

UCLASS()
class PROJECT_RS_API ARSCharacterBase : public ACharacter, public IRSWidgetInterface
{
	GENERATED_BODY()

public:
	ARSCharacterBase();
	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(EditAnywhere, Category = CharacterControl)
	TMap<ECharacterControlType, class URSCharacterControlData*> CharacterControlManager;

	virtual void SetCharacterControlData(const class URSCharacterControlData* ControlData);

// Action Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	TObjectPtr<class URSAttackActionData> AttackActionData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;


	void ProcessAttackCommand();

	void AttackActionBegin();
	void AttackActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);
	void SetComboCheckTimer();
	void ComboCheck();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void SetDead();
	void PlayDeadAnimation();

	int32 CurrentCombo = 0;
	FTimerHandle ComboTimerHandle;
	bool HasNextAttackCommand = false;
	float DeadEventDelayTime = 8.0f;

// Stat Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAcess = "true"))
	TObjectPtr<class URSCharacterStatComponent> Stat;

// UI Widget Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAcess = "true"))
	TObjectPtr<class URSWidgetComponent> HpBar;

	virtual void SetupWidget(class URSUserWidget* InUserWidget) override;
};
