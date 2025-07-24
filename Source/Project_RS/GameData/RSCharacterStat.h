
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RSCharacterStat.generated.h"

USTRUCT(BlueprintType)
struct FRSCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FRSCharacterStat() : MaxHp(0.0f), Attack(0.0f), AttackRange(0.0f), AttackSpeed(0.0f), MovementSpeed(0.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed;

	FRSCharacterStat operator+(const FRSCharacterStat& Other) const
	{
		const float* const ThisPtr = reinterpret_cast<const float*>(this);
		const float* const OtherPtr = reinterpret_cast<const float*>(&Other);

		FRSCharacterStat Result;
		float* const ResultPtr = reinterpret_cast<float*>(&Result);
		int StatNum = sizeof(FRSCharacterStat) / sizeof(float);
		for (int32 i = 0; i < StatNum; ++i)
		{
			ResultPtr[i] = ThisPtr[i] + OtherPtr[i];
		}

		return Result;
	}
};