// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/NotifyState_ActiveAttack.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Enemy/Enemy.h"

void UNotifyState_ActiveAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    //Super::NotifyBegin(MeshComp, Animation, TotalDuration);
    AActor* OwingActor = MeshComp->GetOwner();
    AEnemy* Enemy = Cast<AEnemy>(OwingActor);

    if(Enemy){
        Enemy->ActivateAttack(TotalDuration);
    }else{
        UE_LOG(LogTemp, Warning, TEXT("ActiveAttackNotify Cast Enemy Fail"));
    }
}

void UNotifyState_ActiveAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    // 기본적으로 종료 시 실행할 로직 추가
    UE_LOG(LogTemp, Warning, TEXT("NotifyEnd triggered."));
}