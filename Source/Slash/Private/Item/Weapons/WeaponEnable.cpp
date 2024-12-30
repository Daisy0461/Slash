#include "Item/Weapons/WeaponEnable.h"
#include "Item/Weapons/Weapon.h"

void UWeaponEnable::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    for(AWeapon* Weapon : Weapons){
        if(Weapon){
            Weapon->SetWeaponCollision(true);
        }
    }
}

void UWeaponEnable::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    for(AWeapon* Weapon : Weapons){
        if(Weapon){
            Weapon->SetWeaponCollision(false);
        }
    }
}