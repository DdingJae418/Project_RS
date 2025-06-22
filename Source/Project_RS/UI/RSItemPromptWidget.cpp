// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RSItemPromptWidget.h"
#include "Components/TextBlock.h"
#include "Interface/RSWidgetInterface.h"

URSItemPromptWidget::URSItemPromptWidget(const FObjectInitializer& ObjectInitializer)
{

}

void URSItemPromptWidget::SetItemName(const FText& ItemName)
{
	ItemNameText->SetText(ItemName);
}

void URSItemPromptWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtItemName")));
	ensureMsgf(ItemNameText, TEXT("URSItemPromptWidget::NativeConstruct: ItemNameText is null! Make sure the widget has a TextBlock named TxtItemName."));

	IRSWidgetInterface* ItemWidget = Cast<IRSWidgetInterface>(OwningActor);
	if (ItemWidget)
	{
		ItemWidget->SetupWidget(this);
	}
}
