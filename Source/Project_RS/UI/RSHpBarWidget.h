// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RSUserWidget.h"
#include "RSHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSHpBarWidget : public URSUserWidget
{
	GENERATED_BODY()

public:
	URSHpBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }
	void UpdateHpBar(float NewCurrentHp);
	
protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

private:
	float MaxHp;
};
