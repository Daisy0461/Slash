// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warlord/WarlordEnemy.h"
#include "Enemy/Warlord/WarlordWeapon.h"
#include "Enemy/Warlord/WarlordEnemyAIController.h"
#include "Enemy/EnemyAttacks/EnemyAutoAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyAOEAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyFireBallAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyThrowWeaponAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyTeleportComponent.h"
#include "Item/FloatingPlaform.h"

AWarlordEnemy::AWarlordEnemy()
{
    WarlordWeapon = CreateDefaultSubobject<UWarlordWeapon>(TEXT("WarlordWeapon"));
    WarlordWeapon->bAutoActivate = true;

    EnemyAutoAttackComponent = CreateDefaultSubobject<UEnemyAutoAttackComponent>(TEXT("AutoAttackComponent"));
    EnemyAOEAttackComponent = CreateDefaultSubobject<UEnemyAOEAttackComponent>(TEXT("AOEAttackComponent"));
    EnemyFireBallAttackComponent = CreateDefaultSubobject<UEnemyFireBallAttackComponent>(TEXT("FireBallAttackComponent"));
    EnemyThrowWeaponAttackComponent = CreateDefaultSubobject<UEnemyThrowWeaponAttackComponent>(TEXT("ThrowWeaponAttackComponent"));
    EnemyTeleportComponent = CreateDefaultSubobject<UEnemyTeleportComponent>(TEXT("EnemyTeleportComponent"));
    EnemyTeleportComponent->bAutoActivate = false;
}

void AWarlordEnemy::BeginPlay()
{
    Super::BeginPlay();

    WarlordEnemyAIController = Cast<AWarlordEnemyAIController>(this->GetController());
}

void AWarlordEnemy::EnemyGuard(AActor* AttackActor)
{
    if(!WarlordEnemyAIController){
        UE_LOG(LogTemp, Warning, TEXT("WarlordEnemyAIController Cast Fail"));
        return;
    }

    AActor* Actor = WarlordEnemyAIController->GetAttackTargetActor();
    if(!Actor){     //현재 AttackActor가 뭔지 모름.
        //UE_LOG(LogTemp, Warning, TEXT("Warlord Don't Know AttackTargetActor"));
        return;     //Guard 불가능
    }else if  (WarlordEnemyAIController->GetEnemyState() == EEnemyState::EES_Attacking){
        UE_LOG(LogTemp, Warning, TEXT("Warlord is Attacking"));
        return;     //Guard 불가능
    }else{
        WarlordEnemyAIController->SetEnemyGuardState(EEnemyGuardState::EEGS_Guarding);
        
        ChoosePlayMontageSection(GuardingAnimation, GuardingSection);
        isEnemyGuarding = true;
    }
}

void AWarlordEnemy::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
    UE_LOG(LogTemp, Display, TEXT("Get Hit Warlord"));
    if(SpawnedFloatingPlatform){
        SpawnedFloatingPlatform->Destroy();
    }
    //SetDodgeCollision(ECollisionEnabled::NoCollision);
    if(WarlordEnemyAIController->GetEnemyGuardState() == EEnemyGuardState::EEGS_Guarding || bIsDoGaurd){   
        UE_LOG(LogTemp, Display, TEXT("Get Hit Guard"));
    }else if(GetIsInterruptible() == false){        //방해 불가.
        UE_LOG(LogTemp, Display, TEXT("Get Is Interruptible false"));
        return;
    }else{
        AttackEnd();
        // SetWarriorWeaponCollision(GetWarriorWeapon(), ECollisionEnabled::NoCollision);
        // SetWarriorParryCollision(GetWarriorWeapon(), ECollisionEnabled::NoCollision);

        if(EnemyAutoAttackComponent){
            EnemyAutoAttackComponent->StopAutoAttackMontage();
        }

        if(EnemyAOEAttackComponent){
            EnemyAOEAttackComponent->SpinMeshTimelineControll(false);
        }

	    Super::GetHit_Implementation(ImpactPoint, Hitter);
    }

    if(bIsDoGaurd) bIsDoGaurd = false;
}

float AWarlordEnemy::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
    RandomGaurd();
    if(bIsDoGaurd){
        PlayGuardImpactMontage();
        return 0;
    }else if(WarlordEnemyAIController->GetEnemyGuardState() == EEnemyGuardState::EEGS_Guarding){
        UE_LOG(LogTemp, Display, TEXT("Take Damage Guard"));
        return 0;
    }
    else{
        Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
        return DamageAmount;
    }
}

void AWarlordEnemy::PlayGuardImpactMontage()
{
    if(GuardImpactAnimation){
        ChoosePlayMontageSection(GuardImpactAnimation, GuardImpactSection);
    }else{
        UE_LOG(LogTemp, Warning, TEXT("GuardImpactAnimation is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
    }
}

void AWarlordEnemy::RandomGaurd(){
    int32 RandomChance = FMath::RandRange(0, 99);

    if (RandomChance < RandomGaurdPercent) // 40% 확률
    {
        bIsDoGaurd = true;
    }
}

void AWarlordEnemy::SetWarlordWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
    WarlordWeapon->SetWeaponCollision(CollisionWeapon, CollisionType);
}

void AWarlordEnemy::SetWarlordParryCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
    WarlordWeapon->SetParryBoxCollision(CollisionWeapon, CollisionType);
}

AWeapon* AWarlordEnemy::GetWarlordWeapon()
{
    return WarlordWeapon->GetWeapon();
}

void AWarlordEnemy::SpawnFloatingPlatform()
{
    SpawnedFloatingPlatform = GetWorld()->SpawnActor<AFloatingPlaform>(FloatingPlatform, GetGroundLocation(this), GetActorRotation());
}

void AWarlordEnemy::FloatingPlatformDown()
{
    if(SpawnedFloatingPlatform){
        SpawnedFloatingPlatform->ReverseFloating();
    }
}