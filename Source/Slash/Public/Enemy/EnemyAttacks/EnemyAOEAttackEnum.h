#pragma once
//Enemy Movement Enum

UENUM(BlueprintType)
enum class EEnemyAOEAttackEnum : uint8
{
    EEAA_SpinningAttack UMETA(DisplayName = "SpinningAttack"),
    EEAA_GroundAttack UMETA(DisplayName = "GroundAttack"),
    EEAA_MagicAreaAttack UMETA(DisplayName = "MagicAreaAttack"),
    EEAA_HealingArea UMETA(DisplayName = "HealingArea")
};