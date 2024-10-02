#pragma once
//Enemy Movement Enum

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    EES_Passive UMETA(DisplayName = "Passive"),
    EES_Investing UMETA(DisplayName = "Investing"),
    EES_Chasing UMETA(DisplayName = "Chasing"),
    EES_Strafing UMETA(DisplayName = "Strafing"),
    EES_Attacking UMETA(DisplayName = "Attacking"),
    EES_Parried UMETA(DisplayName = "Parried"),
    EES_Hitting UMETA(DisplayName = "Hitting"),
    EES_Dead UMETA(DisplayName = "Dead"),

    EES_NoState UMETA(DisplayName = "No State")
};