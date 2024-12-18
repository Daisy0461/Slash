// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorEnemy.h"
#include "Enemy/Warrior/WarriorWeapon.h"
#include "Enemy/EnemyEnum/EnemyState.h"
#include "Enemy/Warrior/WarriorEnemyAIController.h"
#include "Interfaces/DodgeInterface.h"
#include "Components/BoxComponent.h"

AWarriorEnemy::AWarriorEnemy()
{
    DodgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PerfectDodge Box"));
    DodgeBox->SetupAttachment(GetRootComponent());
    DodgeBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DodgeBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    DodgeBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

    WarriorWeapon = CreateDefaultSubobject<UWarriorWeapon>(TEXT("WarriorWeapon"));

    AttackRadius = 150.f;
    DefendRadius = 500.f;
}

void AWarriorEnemy::BeginPlay()
{
	Super::BeginPlay();
	
    WarriorEnemyAIController = Cast<AWarriorEnemyAIController>(this->GetController());

    DodgeBox->OnComponentBeginOverlap.AddDynamic(this, &AWarriorEnemy::OnDodgeBoxOverlap);
    DodgeBox->OnComponentEndOverlap.AddDynamic(this, &AWarriorEnemy::OnDodgeBoxEndOverlap);
}

void AWarriorEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWarriorEnemy::AttackByAI()
{
    Super::AttackByAI();   		//Play AutoAttack Montage

}

void AWarriorEnemy::OnDodgeBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    if(OtherActor && OtherActor->ActorHasTag(FName("Enemy"))) return;

    DodgeInterface = Cast<IDodgeInterface>(OtherActor);
    if(DodgeInterface){
        DodgeInterface->SetIsInEnemyAttackArea(true);
    }
}

void AWarriorEnemy::OnDodgeBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // 확인: 다른 액터가 유효하고 본인이 아닌 경우
    if(OtherActor && OtherActor->ActorHasTag(FName("Enemy")))return;

    DodgeInterface = Cast<IDodgeInterface>(OtherActor);
    if(DodgeInterface){
        DodgeInterface->SetIsInEnemyAttackArea(false);
    }

}

void AWarriorEnemy::SetDodgeCollision(ECollisionEnabled::Type CollisionType)
{
	if(DodgeBox){
        DodgeBox->SetCollisionEnabled(CollisionType);
    }
}

void AWarriorEnemy::SetDodgeCharacterIsInEnemyAttackArea()
{
    //Animation이 여기까지 안오면 실행이 안된다.
    if(DodgeInterface){
        DodgeInterface->SetIsInEnemyAttackArea(false);
    }
}

void AWarriorEnemy::SetWarriorWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
    WarriorWeapon->SetWeaponCollision(CollisionWeapon, CollisionType);
}

void AWarriorEnemy::SetWarriorParryCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
    WarriorWeapon->SetParryBoxCollision(CollisionWeapon, CollisionType);
}

AWeapon* AWarriorEnemy::GetWarriorWeapon()
{
    return WarriorWeapon->GetWeapon();
}

AWeapon* AWarriorEnemy::GetWarriorShield()
{
    return WarriorWeapon->GetShield();
}

//viking에서 이 함수를 실행시킴.
void AWarriorEnemy::EnemyGuard(AActor* AttackActor)
{
    //여기서 이 WarriorEnemy가 Pawn의 AI Controller를 AIC_WarriorEnemy를 갖고 있는지 확인해야한다.
    //그렇다는 말은 WarriorEnemyAIController로 변환해야한다. 이것을 기반으로 만든 것이 AIC_BaseEnemyCPP이니까.
    //물론 먼저 변환해놓는게 더 쌀듯 -> BeginPlay() -> OK
    if(!WarriorEnemyAIController){
        UE_LOG(LogTemp, Warning, TEXT("WarriorEnemyAIController Cast Fail"));
        return;
    }

    AActor* Actor = WarriorEnemyAIController->GetAttackTargetActor();
    if(!Actor){     //현재 AttackActor가 뭔지 모름.
        UE_LOG(LogTemp, Warning, TEXT("Warrior Don't Know AttackTargetActor"));
        return;     //Guard 불가능
    }else{
        WarriorEnemyAIController->SetEnemyGuardState(EEnemyGuardState::EEGS_Guarding);
        
        ChoosePlayMontageSection(GuardingAnimation, GuardingSection);
        isEnemyGuarding = true;
    }
}

void AWarriorEnemy::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
    //UE_LOG(LogTemp, Display, TEXT("In Get Hit Warrior Version"));
    SetDodgeCollision(ECollisionEnabled::NoCollision);
    if(WarriorEnemyAIController->GetEnemyGuardState() == EEnemyGuardState::EEGS_Guarding){
        ChoosePlayMontageSection(GuardImpactAnimation, GuardImpactSection);     //데미지 입지 않음.
    }else{
	    Super::GetHit_Implementation(ImpactPoint, Hitter);
    }
}