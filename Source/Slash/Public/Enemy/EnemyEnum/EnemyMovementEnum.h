#pragma once
//Enemy Movement Enum

UENUM(BlueprintType)
enum class EEnemyMovementSpeed : uint8
{
	EEMS_Idle UMETA(DisplayName = "Idle"),
	EEMS_Walk UMETA(DisplayName = "Walk"),
	EEMS_Jogging UMETA(DisplayName = "Jogging"),
	EEMS_Sprinting UMETA(DisplayName = "Sprinting")
};