// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/RSItem.h"
#include "Item/RSItemData.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Physics/RSCollision.h"
#include "UI/RSWidgetComponent.h"
#include "UI/RSItemPromptWidget.h"
#include "interface/RSCharacterItemInterface.h"
#include "Engine/Engine.h"

// Sets default values
ARSItem::ARSItem()
	: FlickerInterval	{ 2.0f }
	, FlickerDuration	{ 0.2f }
	, MaxOpacity		{ 1.0f } 
	, FlickerTimer		{ 0.0f }
	, bIsFlickering		{ false }
{
	PrimaryActorTick.bCanEverTick = true;
	     
	TriggerBox	= CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(CPROFILE_RSTRIGGER);
	TriggerBox->SetBoxExtent(FVector(100.0f));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ARSItem::OnTriggerBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ARSItem::OnTriggerEndOverlap);
	RootComponent = TriggerBox;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(TriggerBox);
	ItemMesh->SetCollisionProfileName(CPROFILE_NOCOLLISION);

	FlickerEffect = CreateDefaultSubobject<URSWidgetComponent>(TEXT("FlickerEffect"));
	FlickerEffect->SetupAttachment(TriggerBox);
	FlickerEffect->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	FlickerEffect->SetWidgetSpace(EWidgetSpace::Screen);
	FlickerEffect->SetDrawSize(FVector2D(140.0f, 140.0f));
	FlickerEffect->SetPivot(FVector2D(0.5f, 0.5f));
	FlickerEffect->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlickerEffect->SetHiddenInGame(true);

	ItemPrompt = CreateDefaultSubobject<URSWidgetComponent>(TEXT("ItemPromptWidget"));
	ItemPrompt->SetWidgetSpace(EWidgetSpace::Screen);
	ItemPrompt->SetDrawSize(FVector2D(200.0f, 80.f));
	ItemPrompt->SetPivot(FVector2D(0.1f, 1.0f));
	ItemPrompt->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemPrompt->SetupAttachment(TriggerBox);
	ItemPrompt->SetRelativeLocation(FVector(0.0f, 0.0f, 10.f));
	ItemPrompt->SetHiddenInGame(true);
}

void ARSItem::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(ItemData, TEXT("[%s] ItemData is not assigned."), *GetName());
	//ensureMsgf(ItemPrompt->GetWidget(), TEXT("[%s] ItemPrompt widget is not assigned."), *GetName());
	//ensureMsgf(FlickerEffect->GetWidget(), TEXT("[%s] FlickerEffect widget is not assigned."), *GetName());
	
	FlickerTimer = FMath::RandRange(0.0f, FlickerInterval);
}

void ARSItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateFlicker(DeltaTime);
}


void ARSItem::ConsumeItem()
{
	SetActorEnableCollision(false);
	ItemPrompt->SetHiddenInGame(true);

	Destroy();
}

void ARSItem::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IRSCharacterItemInterface* ItemUser = Cast<IRSCharacterItemInterface>(OtherActor);
	if (nullptr == ItemUser)
		return;

	ItemUser->FindItem(this);
	ItemPrompt->SetHiddenInGame(false);
}

void ARSItem::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IRSCharacterItemInterface* ItemUser = Cast<IRSCharacterItemInterface>(OtherActor);
	if (nullptr == ItemUser)
		return;

	ItemUser->LoseItem(this);
	ItemPrompt->SetHiddenInGame(true);
}

void ARSItem::SetupWidget(class UUserWidget* InUserWidget)
{
	URSItemPromptWidget* ItemPromptWidget = Cast<URSItemPromptWidget>(InUserWidget);
	if (ItemPromptWidget)
	{
		ItemPromptWidget->SetItemName(ItemData->GetName());
	}
}

void ARSItem::StartFlicker()
{
	bIsFlickering = true;
	FlickerTimer = 0.0f;
}

void ARSItem::UpdateFlicker(float DeltaTime)
{
	FlickerTimer += DeltaTime;
	
	if (false == bIsFlickering)
	{
		if (FlickerInterval < FlickerTimer)
		{
			StartFlicker();
		}
	}
	else
	{
		if (FlickerTimer < FlickerDuration)
		{
			float Alpha = FMath::Sin((FlickerTimer / FlickerDuration) * PI);
			if (FlickerEffect->GetWidget())
			{
				FlickerEffect->GetWidget()->SetRenderOpacity(Alpha * MaxOpacity);
				FlickerEffect->SetHiddenInGame(false);
			}
		}
		else
		{
			FlickerEffect->SetHiddenInGame(true);
			bIsFlickering = false;
			FlickerTimer = 0.0f;
		}
	}
}
