// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_AttackInRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RSAI.h"
#include "Interface/RSCharacterAIInterface.h"
#include "AIController.h"

UBTDecorator_AttackInRange::UBTDecorator_AttackInRange()
{
	NodeName = TEXT("CanAttack");

	bNotifyBecomeRelevant = true;
	bNotifyTick = true;
}

void UBTDecorator_AttackInRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	FAttackInRangeMemory* Mem = reinterpret_cast<FAttackInRangeMemory*>(NodeMemory);
	Mem->bLastResult = CalculateRawConditionValue(OwnerComp, NodeMemory) ? 1 : 0;
}

void UBTDecorator_AttackInRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FAttackInRangeMemory* Mem = reinterpret_cast<FAttackInRangeMemory*>(NodeMemory);
	bool bCur = CalculateRawConditionValue(OwnerComp, NodeMemory);
	bool bPrev = (Mem->bLastResult != 0);

	if (bCur != bPrev)
	{
		OwnerComp.RequestExecution(this);
		Mem->bLastResult = bCur ? 1 : 0;
	}
}

bool UBTDecorator_AttackInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	const IRSCharacterAIInterface* AIPawn = Cast<IRSCharacterAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return false;
	}

	const float AttackRange = AIPawn->GetAIAttackRange();
	const float TargetDist = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(BBKEY_TARGETDIST);
	return TargetDist <= AttackRange;
}
