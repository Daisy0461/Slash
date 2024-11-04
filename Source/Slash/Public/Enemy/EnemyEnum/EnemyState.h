#pragma once
//Enemy Movement Enum

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    EES_Passive UMETA(DisplayName = "Passive"),         //0
    EES_Investing UMETA(DisplayName = "Investing"),     //1
    EES_Chasing UMETA(DisplayName = "Chasing"),         //2
    EES_Strafing UMETA(DisplayName = "Strafing"),       //3
    EES_Attacking UMETA(DisplayName = "Attacking"),     //5
    EES_Parried UMETA(DisplayName = "Parried"),         //6
    EES_Hitting UMETA(DisplayName = "Hitting"),         //7
    EES_Dead UMETA(DisplayName = "Dead"),               //8

    EES_NoState UMETA(DisplayName = "No State")         //9
};

UENUM(BlueprintType)
enum class EEnemyGuardState : uint8
{
    EEGS_NoGuard UMETA(DisplayName = "No Guarding"),
    EEGS_Guarding UMETA(DisplayName = "Guarding"),
    EEGS_GuardComplete UMETA(DisplayName = "Guard Complete")
};