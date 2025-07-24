// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RSCharacterStat.h"
#include "Enums/ECharacterName.h"
#include "RSGameSingleton.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRSGameSingleton, Error, All);

/**
 * 
 */
UCLASS()
class PROJECT_RS_API URSGameSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	URSGameSingleton();
	
	static URSGameSingleton& Get();

	FORCEINLINE FRSCharacterStat GetCharacterStat(ECharacterName CharacterName) const { return CharacterStatTable.Contains(CharacterName) ? CharacterStatTable[CharacterName] : FRSCharacterStat(); }

	UPROPERTY()
	int32 CharacterMaxNum;

private:
	TMap<ECharacterName, FRSCharacterStat> CharacterStatTable;
};
