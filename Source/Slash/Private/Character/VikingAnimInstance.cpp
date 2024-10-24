// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/VikingAnimInstance.h"
#include "Character/VikingCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

//ABP를 삭제하지 않고 그대로 남겨뒀다.
void UVikingAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    VikingCharacter = Cast<AVikingCharacter>(TryGetPawnOwner());
    if(VikingCharacter)
    {   
        //VikingCharacter가 ACharacter를 상속받기 때문에 GetCharacterMovement가 사용 가능하다.
        VikingCharacterMovement = VikingCharacter->GetCharacterMovement();
    }
}

void UVikingAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if(VikingCharacterMovement){  
        //VSizeXY는 XY축에 관하여 Vector의 Size를 받는 것이고 Velocity는 속도이다.
        //uKismetMathLibrary의 경우 Class에 있는 함수가 파라미터를 조정하지 않고 계산한 과정을 Return한다.
        GroundSpeed = UKismetMathLibrary::VSizeXY(VikingCharacterMovement->Velocity);
        CalculateDirection();
        IsFalling = VikingCharacterMovement->IsFalling();
        CharacterState = VikingCharacter->GetCharacterState();
        ActionState = VikingCharacter->GetActionState();
        Aim = CalculateAimOffset();
    }
}

void UVikingAnimInstance::CalculateDirection()
{
    FVector Velocity = VikingCharacter->GetVelocity();
    FRotator ActorRotation = VikingCharacter->GetActorRotation();
    
    FVector ForwardVector = ActorRotation.Vector(); // 캐릭터의 앞쪽 벡터 (X축)
    FVector RightVector = FRotationMatrix(ActorRotation).GetUnitAxis(EAxis::Y); // 캐릭터의 오른쪽 벡터 (Y축)

    // ForwardVector와 RightVector를 사용하여 속도가 어느 방향으로 향하는지 계산
    float ForwardValue = FVector::DotProduct(ForwardVector, Velocity); // 앞쪽 방향 속도
    float RightValue = FVector::DotProduct(RightVector, Velocity);     // 오른쪽 방향 속도

    // atan2를 사용해 속도의 방향을 각도로 변환 (라디안 -> 도로 변환)
    Direction = FMath::Atan2(RightValue, ForwardValue) * (180.0f / PI);
}

float UVikingAnimInstance::CalculateAimOffset()
{
    float DeltaRotationPitch = 0.f;
    if(VikingCharacter->GetController()){
        FRotator ActorRotation = VikingCharacter->GetActorRotation();
        FRotator AimRotaion = VikingCharacter->GetAimRotation();

        FRotator DeltaRotation = (AimRotaion - ActorRotation).GetNormalized();
        DeltaRotationPitch = DeltaRotation.Pitch;
    }

    return DeltaRotationPitch;
}
  