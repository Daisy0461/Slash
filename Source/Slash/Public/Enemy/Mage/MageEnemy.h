// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyEnum/EnemyMovementEnum.h"
#include "MageEnemy.generated.h"

class USceneComponent; 
class USkeletalMeshComponent;
class UMaterialInstanceDynamic;
class UNiagaraComponent;
class UNiagaraSystem;
class UEnemyFireBallAttackComponent;
class UEnemyTeleportComponent;
class AEnemyAOEAttack;
class AEnemyAreaHeal;

UCLASS()
class SLASH_API AMageEnemy : public AEnemy
{
	GENERATED_BODY()
public:
	AMageEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	//Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UEnemyAOEAttackComponent* EnemyAOEAttackComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UEnemyFireBallAttackComponent* EnemyFireBallComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UEnemyTeleportComponent* EnemyTeleportComponent;

};
