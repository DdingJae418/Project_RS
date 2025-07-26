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
{
	PrimaryActorTick.bCanEverTick = true;
	
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	Mesh	= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;

	Trigger->SetCollisionProfileName(CPROFILE_RSTRIGGER);
	Trigger->SetBoxExtent(FVector(100.0f));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ARSItem::OnTriggerBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ARSItem::OnTriggerEndOverlap);

	Mesh->SetupAttachment(Trigger);
	Mesh->SetCollisionProfileName("NoCollision");

	FlickerEffect = CreateDefaultSubobject<URSWidgetComponent>(TEXT("FlickerEffect"));
	static ConstructorHelpers::FClassFinder<UUserWidget> FlickerWidgetRef(TEXT("/Game/Project_RS/UI/WBP_ItemFlicker.WBP_ItemFlicker_C"));
	if (FlickerWidgetRef.Class)
	{
		FlickerEffect->SetWidgetClass(FlickerWidgetRef.Class);
		FlickerEffect->SetupAttachment(Trigger);
		FlickerEffect->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		FlickerEffect->SetWidgetSpace(EWidgetSpace::Screen);
		FlickerEffect->SetDrawSize(FVector2D(140.0f, 140.0f));
		FlickerEffect->SetPivot(FVector2D(0.5f, 0.5f));
		FlickerEffect->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FlickerEffect->SetHiddenInGame(true);
	}

	FlickerTimer = 0.0f;
	bIsFlickering = false;

	ItemPrompt = CreateDefaultSubobject<URSWidgetComponent>(TEXT("Widget"));
	static ConstructorHelpers::FClassFinder<UUserWidget> ItemPromptWidgetRef(TEXT("/Game/Project_RS/UI/WBP_ItemPrompt.WBP_ItemPrompt_C"));
	if (ItemPromptWidgetRef.Class)
	{
		ItemPrompt->SetWidgetClass(ItemPromptWidgetRef.Class);
		ItemPrompt->SetWidgetSpace(EWidgetSpace::Screen);
		ItemPrompt->SetDrawSize(FVector2D(200.0f, 80.f));
		ItemPrompt->SetPivot(FVector2D(0.1f, 1.0f));
		ItemPrompt->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemPrompt->SetupAttachment(Trigger);
		ItemPrompt->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
		ItemPrompt->SetHiddenInGame(true);
	}
}

void ARSItem::BeginPlay()
{
	Super::BeginPlay();
	ensureMsgf(ItemData, TEXT("[%s] ItemData �� �Ҵ���� �ʾҽ��ϴ�."), *GetName());
	
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
	UE_LOG(LogTemp, Log, TEXT("%s"), *ItemData->GetName().ToString());

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
		if (FlickerTimer >= FlickerInterval)
		{
			StartFlicker();
		}
	}
	else
	{
		if (FlickerTimer <= FlickerDuration)
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
