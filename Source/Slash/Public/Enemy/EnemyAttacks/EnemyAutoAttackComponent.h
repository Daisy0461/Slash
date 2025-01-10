// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAutoAttackComponent.generated.h"

class UAnimMontage;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UEnemyAutoAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyAutoAttackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	AActor* OwnerActor;
	UPROPERTY(EditAnywhere, Category = "Montage")
	UAnimMontage* AutoAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AutoAttackMontageSection;		//Section 추가 안하면 재생안함. -> 0개일 경우 Warning추가.

		
};
