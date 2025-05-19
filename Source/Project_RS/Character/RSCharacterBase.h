// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RSCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8 
{
	Shoulder,
	Aiming
};

UCLASS()
class PROJECT_RS_API ARSCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ARSCharacterBase();

protected:
	virtual void SetCharacterControlData(const class URSCharacterControlData* ControlData);

	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class URSCharacterControlData*> CharacterControlManager;
};
