// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_AttackInRange.generated.h"

USTRUCT()
struct FAttackInRangeMemory
{
    GENERATED_BODY()
    uint8 bLastResult : 1;
};

/**
 * 
 */
UCLASS()
class PROJECT_RS_API UBTDecorator_AttackInRange : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_AttackInRange();

protected:protected:
    virtual uint16 GetInstanceMemorySize() const override { return sizeof(FAttackInRangeMemory); }

    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

};
