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
        IsFalling = VikingCharacterMovement->IsFalling();
        CharacterState = VikingCharacter->GetCharacterState();
        ActionState = VikingCharacter->GetActionState();
    }
}
 