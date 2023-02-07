// Fill out your copyright notice in the Description page of Project Settings.


#include "Charater/ParagonAnimInstance.h"
#include "Charater/ParagonCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


//ABP를 삭제하지 않고 그대로 남겨뒀다.
void UParagonAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    ParagonCharacter = Cast<AParagonCharacter>(TryGetPawnOwner());
    if(ParagonCharacter)
    {   
        //ParagonCharacter가 ACharacter를 상속받기 때문에 GetCharacterMovement가 사용 가능하다.
        ParagonCharacterMovement = ParagonCharacter->GetCharacterMovement();
    }
}

void UParagonAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if(ParagonCharacterMovement){  
        //VSizeXY는 XY축에 관하여 Vector의 Size를 받는 것이고 Velocity는 속도이다.
        //uKismetMathLibrary의 경우 Class에 있는 함수가 파라미터를 조정하지 않고 계산한 과정을 Return한다.
        GroundSpeed = UKismetMathLibrary::VSizeXY(ParagonCharacterMovement->Velocity);
        IsFalling = ParagonCharacterMovement->IsFalling();
    }
}
