// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/RSUserWidget.h"
#include "RSItemPromptWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSItemPromptWidget : public URSUserWidget
{
	GENERATED_BODY()
	
public:
	URSItemPromptWidget(const FObjectInitializer& ObjectInitializer);

	void SetItemName(const FText& ItemName);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<class UTextBlock> ItemNameText;
};
