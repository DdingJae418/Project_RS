// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RSCharacterBase.generated.h"

// Forward Declarations
class UAnimMontage;
class URSAttackActionData;
class URSCharacterControlData;
class URSCharacterStatComponent;
class USoundCue;

UENUM(BlueprintType)
enum class ECharacterControlType : uint8 
{
	Shoulder	UMETA(DisplayName = "Shoulder View"),
	Aiming		UMETA(DisplayName = "Aiming View")
};

/**
 * Base character class for all characters in the game.
 * Provides core functionality for combat, stats, and UI integration.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class PROJECT_RS_API ARSCharacterBase : public ACharacter
{
	GENERATED_BODY()

// ================================================================================================
// PUBLIC INTERFACE
// ================================================================================================
public:
	// Constructor & Core Overrides
	ARSCharacterBase();
	virtual void PostInitializeComponents() override;

	// Combat Interface
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void ProcessAttackCommand();

	// Stat Interface
	UFUNCTION(BlueprintPure, Category = "Stats")
	FORCEINLINE URSCharacterStatComponent* GetStatComponent() const { return Stat; }



// ================================================================================================
// PROTECTED IMPLEMENTATION
// ================================================================================================
protected:
	// ========== Character Control System ==========
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Control", meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, TObjectPtr<URSCharacterControlData>> CharacterControlManager;

	virtual void SetCharacterControlData(const URSCharacterControlData* ControlData);

	// ========== Combat System ==========
	// Combat Data & Assets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AttackActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DeadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Data", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSAttackActionData> AttackActionData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Sound")
	TObjectPtr<USoundCue> AttackSound;


	// Combat Methods
	virtual void AttackActionBegin();
	virtual void AttackActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded);
	virtual void NotifyAttackActionEnd();
	virtual void SetComboCheckTimer();
	virtual void ComboCheck();
	void PlayAttackSound();

	// Damage & Death System
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetDead();
	virtual void PlayDeadAnimation();
	float GetDeadEventDelayTime() const { return DeadEventDelayTime_; }

	// ========== Stat System ==========
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URSCharacterStatComponent> Stat;



// ================================================================================================
// PRIVATE IMPLEMENTATION
// ================================================================================================
private:
	// ========== Combat Internal State ==========
	int32	CurrentCombo_			= 0;
	bool	HasNextAttackCommand_	= false;
	bool	IsDead_					= false;
	float	DeadEventDelayTime_		= 8.0f;

	FTimerHandle ComboTimerHandle_;
};
