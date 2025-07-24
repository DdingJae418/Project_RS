// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/RSGameSingleton.h"

DEFINE_LOG_CATEGORY(LogRSGameSingleton);

URSGameSingleton::URSGameSingleton()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Script/Engine.DataTable'/Game/Project_RS/GameData/RSCharacterStatTable.RSCharacterStatTable'"));
	if (nullptr != DataTableRef.Object)
	{
		const UDataTable* DataTable = DataTableRef.Object;
		check(DataTable->GetRowMap().Num() > 0);

		const UEnum* CharacterNameEnum = StaticEnum<ECharacterName>();
		check(CharacterNameEnum != nullptr);

		for (const auto& Row : DataTable->GetRowMap())
		{
			int64 NameValue = CharacterNameEnum->GetValueByName(Row.Key);
			if (NameValue == INDEX_NONE)
			{
				UE_LOG(LogRSGameSingleton, Warning, TEXT("'%s' 에 해당하는 ECharacterName 값이 없습니다."), *Row.Key.ToString());
			}
			else
			{
				ECharacterName CharacterName = static_cast<ECharacterName>(NameValue);
				const FRSCharacterStat* Stat = reinterpret_cast<const FRSCharacterStat*>(Row.Value);
				if (Stat)
				{
					CharacterStatTable.Add(CharacterName, *Stat);
				}
			}
		}

	}
}

URSGameSingleton& URSGameSingleton::Get()
{
	URSGameSingleton* Singleton = CastChecked<URSGameSingleton>(GEngine->GameSingleton);
	if (Singleton)
	{
		return *Singleton;
	}

	UE_LOG(LogRSGameSingleton, Error, TEXT("URSGameSingleton is not initialized!"));
	return *NewObject<URSGameSingleton>();
}