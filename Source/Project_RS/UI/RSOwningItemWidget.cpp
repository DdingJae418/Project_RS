// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RSOwningItemWidget.h"
#include "Components/TextBlock.h"

URSOwningItemWidget::URSOwningItemWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxAmmo_ = 0;
}

void URSOwningItemWidget::SetMaxAmmo(uint8 NewMaxAmmo)
{
	check(MaxAmmoText_);

	MaxAmmo_ = NewMaxAmmo;
	MaxAmmoText_->SetText(FText::AsNumber(MaxAmmo_));
}

void URSOwningItemWidget::UpdateOwningAmmo(uint8 NewCurrentAmmo)
{
	check(OwningAmmoText_);
	ensure(MaxAmmo_ > 0);

	uint8 CurrentAmmo = FMath::Clamp(NewCurrentAmmo, 0, MaxAmmo_);
	OwningAmmoText_->SetText(FText::AsNumber(CurrentAmmo));
}

void URSOwningItemWidget::UpdateOwningMoney(uint32 NewCurrentMoney)
{
	check(OwningMoneyText_);

	OwningMoneyText_->SetText(FText::AsNumber(NewCurrentMoney));
}

void URSOwningItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OwningAmmoText_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtOwningAmmo")));
	check(OwningAmmoText_);

	MaxAmmoText_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtMaxAmmo")));
	check(MaxAmmoText_);

	OwningMoneyText_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtOwningMoney")));
	check(OwningMoneyText_);
}
