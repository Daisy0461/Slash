// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/AnimNotify_PlayMontageNotify.h"
#include "NotifyState_ActiveAttack.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UNotifyState_ActiveAttack : public UAnimNotify_PlayMontageNotifyWindow
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
    //virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
};
