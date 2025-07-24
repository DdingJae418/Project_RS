// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RSHUDWidget.h"
#include "Interface/RSWidgetInterface.h"
#include "UI/RSHpBarWidget.h"
#include "UI/RSOwningItemWidget.h"

URSHUDWidget::URSHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void URSHUDWidget::SetMaxHp(float NewMaxHp)
{
	HpBar->SetMaxHp(NewMaxHp);
}

void URSHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	HpBar->UpdateHpBar(NewCurrentHp);
}

void URSHUDWidget::SetMaxAmmo(uint8 newMaxAmmo)
{
	OwningItem->SetMaxAmmo(newMaxAmmo);
}

void URSHUDWidget::UpdateOwningAmmo(uint8 NewCurrentAmmo)
{
	OwningItem->UpdateOwningAmmo(NewCurrentAmmo);
}

void URSHUDWidget::UpdateOwningMoney(uint32 NewCurrentMoney)
{
	OwningItem->UpdateOwningMoney(NewCurrentMoney);
}

void URSHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpBar = Cast<URSHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensure(HpBar);

	OwningItem = Cast<URSOwningItemWidget>(GetWidgetFromName(TEXT("WidgetOwningItem")));
	ensure(OwningItem);

	IRSWidgetInterface* HUDPawn = Cast<IRSWidgetInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupWidget(this);
	}
}
