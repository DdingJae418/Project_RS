// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RSWidgetComponent.h"
#include "UI/RSUserWidget.h"

void URSWidgetComponent::InitWidget()
{
	Super::InitWidget();

	URSUserWidget* RSUserWidget = Cast<URSUserWidget>(GetWidget());
	if (RSUserWidget)
	{
		RSUserWidget->SetOwningActor(GetOwner());
	}
}
