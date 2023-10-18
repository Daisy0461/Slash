// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/CharacterTypes.h"
#include "VikingAnimInstance.generated.h"


class AVikingCharacter;
class UCharacterMovementComponent;

UCLASS()
class SLASH_API UVikingAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;					//애니매이션을 처음 초기화하는 역할
	virtual void NativeUpdateAnimation(float DeltaTime) override;		//애니매이션에서 Tick과 같은 역할
	
	UPROPERTY(BlueprintReadOnly)
	AVikingCharacter* VikingCharacter;							//VikingCharacter를 받아옴

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	UCharacterMovementComponent* VikingCharacterMovement;			//VikingCharacter의 MovementComponent를 들고 옴.

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;												//걸음 속도

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	ECharacterState CharacterState;
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	EActionState ActionState;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EGuardState GuardState;
};
