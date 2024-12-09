// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAOEBase.h"
#include "EnemyAreaHeal.generated.h"

class AEnemy;

UCLASS()
class SLASH_API AEnemyAreaHeal : public AEnemyAOEBase
{
	GENERATED_BODY()
public:
	AEnemyAreaHeal();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;	
	virtual void OnAOECapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;	
	UFUNCTION()
	virtual void OnAOECapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UPROPERTY(EditAnywhere, Category = "AOE")
	float HealAmount = 3.f;
	void InitializeOverlappingEnemies();
	TArray<AEnemy*> HealingEnemyActors;
};
