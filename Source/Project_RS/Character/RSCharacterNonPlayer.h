// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RSCharacterBase.h"
#include "Enums/ECharacterName.h"
#include "Interface/RSCharacterAIInterface.h"
#include "Interface/RSAnimationAttackInterface.h"
#include "RSCharacterNonPlayer.generated.h"

// Forward Declarations
class URSWidgetComponent;

/**
 * AI-controlled character class for NPCs and enemies.
 * Extends base character with AI behavior, patrol systems, and automated combat.
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECT_RS_API ARSCharacterNonPlayer : public ARSCharacterBase, public IRSCharacterAIInterface, public IRSAnimationAttackInterface
{
	GENERATED_BODY()

public:
	ARSCharacterNonPlayer();

	//~ Start AActor interface
	virtual void PostInitializeComponents() override;
	virtual void PostNetInit() override;
	//~ End AActor interface
	 
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	ECharacterName CharacterName;

	//~ Start ARSCharacterBase interface
	virtual void SetDead() override;
	virtual void NotifyAttackActionEnd() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;;
	//~ End ARSCharacterBase interface

	//~ Start IRSAnimationAttackInterface interface
	virtual void AttackHitCheck_Implementation() override;
	//~ End IRSAnimationAttackInterface interface

	//~ Start IRSCharacterAIInterface interface
	virtual float GetAIPatrolRadius() const override;
	virtual float GetAIDetectRange() const override;
	virtual float GetAIAttackRange() const override;
	virtual float GetAITurnSpeed() const override;
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InDelegate) override;
	virtual void AttackByAI() override;
	//~ End IRSCharacterAIInterface interface

private:
	FAICharacterAttackFinished OnAttackFinished;
};
