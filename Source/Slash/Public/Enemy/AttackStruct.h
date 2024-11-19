// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackStruct.generated.h"

/**
 * 
 */
class SLASH_API AttackStruct
{
public:
	AttackStruct();
	~AttackStruct();
};

USTRUCT(Atomic, BlueprintType)
struct FAttackInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Attacker;

	// 기본 생성자
    FAttackInfo()
        : StartTime(0.0f), Duration(0.0f), Attacker(nullptr)
    {}

    // 사용자 정의 값으로 초기화하는 생성자
    FAttackInfo(float InStartTime, float InDuration, AActor* InAttacker)
        : StartTime(InStartTime), Duration(InDuration), Attacker(InAttacker)
    {}

	//Remove를 위해서 만든다.
	bool operator==(const FAttackInfo& Other) const
	{
		// Compare each member variable
		return FMath::IsNearlyEqual(StartTime, Other.StartTime) &&		//연산의 오차를 고려해서 IsNearlyEqual로 함.
			   FMath::IsNearlyEqual(Duration, Other.Duration) &&
			   Attacker == Other.Attacker;
	}
};
