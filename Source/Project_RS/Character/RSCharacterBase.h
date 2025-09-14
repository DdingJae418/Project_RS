// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/RSWidgetInterface.h"
#include "RSCharacterBase.generated.h"


class UAnimMontage;
class URSAttackActionData;
class URSCharacterControlData;
class URSCharacterStatComponent;
class USoundCue;
class URSWidgetComponent;


UENUM(BlueprintType)
enum class ECharacterControlType : uint8 
{
	Shoulder	UMETA(DisplayName = "Shoulder View"),
	Aiming		UMETA(DisplayName = "Aiming View")
};


UCLASS(Abstract, BlueprintType, Blueprintable)
class PROJECT_RS_API ARSCharacterBase : public ACharacter, public IRSWidgetInterface
{
	GENERATED_BODY()

public:
	ARSCharacterBase();

	//~ Start AActor interface
	virtual void PostInitializeComponents() override;
	virtual void PostNetInit() override;
	//~ End AActor interface

	//~ Start IRSWidgetInterface interface
	virtual void SetupWidget(UUserWidget* InUserWidget) override;
	//~ End IRSWidgetInterface interface

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPCProcessAttack();

	UFUNCTION(NetMulticast,Unreliable)
	void MulticastRPCProcessAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetHpBarVisibility(bool bVisible);

	FORCEINLINE URSCharacterStatComponent* GetStatComponent() const { return Stat; }
	FORCEINLINE URSWidgetComponent* GetHpBarComponent() const { return HpBar; }

	virtual void SetCharacterControlData(const URSCharacterControlData* ControlData);
	TMap<ECharacterControlType, TObjectPtr<URSCharacterControlData>> GetCharacterControlManager() const { return CharacterControlManager; }

	virtual void AttackActionBegin();
	virtual void AttackActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded);
	virtual void NotifyAttackActionEnd();
	virtual void SetComboCheckTimer();
	virtual void ComboCheck();
	void PlayAttackSound();

	void PlayDeadAnimation();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetDead();
	float GetDeadEventDelayTime() const { return DeadEventDelayTime; }
	
	void SetupHpBarComponent();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Control", Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, TObjectPtr<URSCharacterControlData>> CharacterControlManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSCharacterStatComponent> Stat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSWidgetComponent> HpBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AttackActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Animation", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DeadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Data", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSAttackActionData> AttackActionData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Sound", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundCue> AttackSound;

	FTimerHandle ComboTimerHandle;
	int32 CurrentCombo = 0;
	bool HasNextAttackCommand = false;
	bool bIsDead = false;
	float DeadEventDelayTime = 8.0f;
};
