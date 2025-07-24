// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RSOwningItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSOwningItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSOwningItemWidget(const FObjectInitializer& ObjectInitializer);

	void SetMaxAmmo(uint8 NewMaxAmmo);
	void UpdateOwningAmmo(uint8 NewCurrentAmmo);
	void UpdateOwningMoney(uint32 NewCurrentMoney);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	TObjectPtr<class UTextBlock> OwningAmmoText_;

	UPROPERTY()
	TObjectPtr<class UTextBlock> MaxAmmoText_;

	UPROPERTY()
	TObjectPtr<class UTextBlock> OwningMoneyText_;

	uint8 MaxAmmo_;
};
