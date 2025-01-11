#pragma once
//Enemy Movement Enum

UENUM(BlueprintType)
enum class EEnemyFireBallEnum : uint8
{
	EFBE_BasicFireBall UMETA(DisplayName = "BasicFireBall"),
    EFBE_BarrageFireBall UMETA(DisplayName = "BarrageFireBall")
};