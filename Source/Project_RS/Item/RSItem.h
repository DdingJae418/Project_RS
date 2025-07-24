// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/RSWidgetInterface.h"
#include "RSItem.generated.h"

UCLASS()
class PROJECT_RS_API ARSItem : public AActor, public IRSWidgetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARSItem();

	virtual void BeginPlay() override;

	FORCEINLINE class URSItemData* GetItemData() const { return ItemData.Get(); }
	void ConsumeItem();

protected:
	UPROPERTY(VisibleAnywhere, Category = Item)
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = Item)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = Item)
	TObjectPtr<class URSWidgetComponent> ItemPrompt;

	UPROPERTY(EditAnywhere, Category = Item, meta = (RequiredComponent))
	TObjectPtr<class URSItemData> ItemData;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void SetupWidget(class UUserWidget* InUserWidget) override;
};
