// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RSOldKey.generated.h"

UCLASS()
class PROJECT_RS_API ARSOldKey : public AActor
{
	GENERATED_BODY()
	
public:	
	ARSOldKey();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Body;
};
