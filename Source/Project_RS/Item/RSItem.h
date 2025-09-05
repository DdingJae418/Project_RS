// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/RSWidgetInterface.h"
#include "Components/TimelineComponent.h"
#include "RSItem.generated.h"

class URSItemData;

UCLASS()
class PROJECT_RS_API ARSItem : public AActor, public IRSWidgetInterface
{
	GENERATED_BODY()
	
public:	
	ARSItem();

	FORCEINLINE URSItemData* GetItemData() const { return ItemData.Get(); }
	void ConsumeItem();

	//~ Start AActor interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//~ End AActor interface

protected:
	//~ Start IRSWidgetInterface interface
	virtual void SetupWidget(class UUserWidget* InUserWidget) override;
	//~ End IRSWidgetInterface interface

private:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<class UBoxComponent> TriggerBox;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<class UStaticMeshComponent> ItemMesh;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<class URSWidgetComponent> ItemPrompt;

	UPROPERTY(EditAnywhere, Category = "Item", meta = (RequiredComponent))
	TObjectPtr<class URSItemData> ItemData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<class URSWidgetComponent> FlickerEffect;

	void StartFlicker();
	void UpdateFlicker(float DeltaTime);

	const float FlickerInterval = 2.0f;
	const float FlickerDuration = 0.3f;
	const float MaxOpacity		= 1.0f;

	float FlickerTimer;
	bool bIsFlickering;
};
