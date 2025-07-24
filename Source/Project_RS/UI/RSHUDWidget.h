// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RSHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSHUDWidget(const FObjectInitializer& ObjectInitializer);

	void SetMaxHp(float NewMaxHp);
	void UpdateHpBar(float NewCurrentHp);
	void SetMaxAmmo(uint8 newMaxAmmo);
	void UpdateOwningAmmo(uint8 NewCurrentAmmo);
	void UpdateOwningMoney(uint32 NewCurrentMoney);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<class URSHpBarWidget> HpBar;

	UPROPERTY()
	TObjectPtr<class URSOwningItemWidget> OwningItem;
};
