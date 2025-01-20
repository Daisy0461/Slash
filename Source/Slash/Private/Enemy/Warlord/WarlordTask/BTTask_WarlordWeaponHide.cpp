// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warlord/WarlordTask/BTTask_WarlordWeaponHide.h"
#include "Enemy/Warlord/WarlordEnemy.h"
#include "Item/Weapons/Weapon.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_WarlordWeaponHide::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(nullptr == ControllingPawn){
        UE_LOG(LogTemp, Display, TEXT("BTTask_WarloUBTTask_WarlordWeaponHide Can't find Pawn"));
        return EBTNodeResult::Failed;
    }

    AWarlordEnemy* OwnerWarlordEnemy = Cast<AWarlordEnemy>(ControllingPawn);
    if(nullptr == OwnerWarlordEnemy){
        UE_LOG(LogTemp, Display, TEXT("BTTask_WarloUBTTask_WarlordWeaponHide Cast Failed"));
        return EBTNodeResult::Failed;
    }

    //OwnerWarlordEnemy->ShortRangeAttack();
    AWeapon* WarlordWeapon = OwnerWarlordEnemy->GetWarlordWeapon();
    WarlordWeapon->ItemMesh->SetVisibility(!bIsHideWeapon);
    return EBTNodeResult::Succeeded; 
}