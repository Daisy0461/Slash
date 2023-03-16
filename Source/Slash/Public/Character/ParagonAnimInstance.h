// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/CharacterTypes.h"
#include "ParagonAnimInstance.generated.h"

/**
 * 
 */
class AParagonCharacter;
class UCharacterMovementComponent;
UCLASS()
class SLASH_API UParagonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;					//애니매이션을 처음 초기화하는 역할
	virtual void NativeUpdateAnimation(float DeltaTime) override;		//애니매이션에서 Tick과 같은 역할
	
	UPROPERTY(BlueprintReadOnly)
	AParagonCharacter* ParagonCharacter;							//Paragon Character를 받아옴

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	UCharacterMovementComponent* ParagonCharacterMovement;			//ParagonCharacter의 MovementComponent를 들고 옴.

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;												//땅에서의 속도

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | Character State")
	ECharacterState CharacterState;
};
