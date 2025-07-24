// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RSCharacterBase.h"
#include "Enums/ECharacterName.h"
#include "Interface/RSCharacterAIInterface.h"
#include "Interface/RSAnimationAttackInterface.h"
#include "RSCharacterNonPlayer.generated.h"

/**
 * AI-controlled character class for NPCs and enemies.
 * Extends base character with AI behavior, patrol systems, and automated combat.
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECT_RS_API ARSCharacterNonPlayer : public ARSCharacterBase, public IRSCharacterAIInterface, public IRSAnimationAttackInterface
{
	GENERATED_BODY()

// ================================================================================================
// PUBLIC INTERFACE
// ================================================================================================
public:
	// Constructor & Core Overrides
	ARSCharacterNonPlayer();
	virtual void PostInitializeComponents() override;

	// Character Interface
	UFUNCTION(BlueprintPure, Category = "Character")
	ECharacterName GetCharacterName() const { return CharacterName; }


// ================================================================================================
// PROTECTED IMPLEMENTATION
// ================================================================================================
protected:
	// ========== Character Data ==========
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	ECharacterName CharacterName;

	// ========== Combat System ==========
	virtual void SetDead() override;
	virtual void NotifyAttackActionEnd() override;

	// IRSAnimationAttackInterface Implementation
	virtual void AttackHitCheck_Implementation() override;

	// ========== AI System (IRSCharacterAIInterface Implementation) ==========
	// AI Configuration
	virtual float GetAIPatrolRadius() const override;
	virtual float GetAIDetectRange() const override;
	virtual float GetAIAttackRange() const override;
	virtual float GetAITurnSpeed() const override;

	// AI Combat Interface
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InDelegate) override;
	virtual void AttackByAI() override;


// ================================================================================================
// PRIVATE IMPLEMENTATION
// ================================================================================================
private:
	// ========== AI Internal State ==========
	FAICharacterAttackFinished OnAttackFinished;
};
