#pragma once

UENUM(BlueprintType)
enum class EEnemyTeleportEnum : uint8
{
    EETE_StartTeleport UMETA(DisplayName = "StartTeleport"),
    EETE_EndTeleport UMETA(DisplayName = "EndTeleport"),
	EETE_ActivateTeleportNiagara UMETA(DisplayName = "ActivateTeleportNiagara"),
    EETE_DeactivateTeleportNiagara UMETA(DisplayName = "DeactivateTeleportNiagaraEnd")
};