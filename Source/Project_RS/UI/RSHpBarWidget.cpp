// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RSHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/RSWidgetInterface.h"

URSHpBarWidget::URSHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.0f;
}

void URSHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();;

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensureMsgf(HpProgressBar, TEXT("URSHpBarWidget: PbHpBar not found in the widget hierarchy."));

	IRSWidgetInterface* CharacterWidget = Cast<IRSWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupWidget(this);
	}
}

void URSHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	ensureMsgf(MaxHp > 0.0f, TEXT("MaxHp must be greater than 0 before updating the HP bar."));

	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
}
