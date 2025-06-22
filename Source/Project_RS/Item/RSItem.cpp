// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/RSItem.h"
#include "Item/RSItemData.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Physics/RSCollision.h"
#include "UI/RSWidgetComponent.h"
#include "UI/RSItemPromptWidget.h"
#include "interface/RSCharacterItemInterface.h"

// Sets default values
ARSItem::ARSItem()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	Mesh	= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;

	Trigger->SetCollisionProfileName(CPROFILE_RSTRIGGER);
	Trigger->SetBoxExtent(FVector(100.0f));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ARSItem::OnTriggerBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ARSItem::OnTriggerEndOverlap);

	Mesh->SetupAttachment(Trigger);
	Mesh->SetCollisionProfileName("NoCollision");

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
	ensureMsgf(ItemData, TEXT("[%s] ItemData 가 할당되지 않았습니다."), *GetName());
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

void ARSItem::SetupWidget(class URSUserWidget* InUserWidget)
{
	URSItemPromptWidget* ItemPromptWidget = Cast<URSItemPromptWidget>(InUserWidget);
	if (ItemPromptWidget)
	{
		ItemPromptWidget->SetItemName(ItemData->GetName());
	}
}
