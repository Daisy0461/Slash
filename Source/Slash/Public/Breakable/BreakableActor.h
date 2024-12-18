// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class UCapsuleComponent;

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void GetHeadShot(FVector ImpactPoint) override;
	virtual void GetHitAOEAttack();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Capsule;

public:	
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;
	UPROPERTY(EditAnywhere, Category = "Treasure")
	TSubclassOf<class ATreasure> TreasureClass;

	UFUNCTION()
	virtual void SpawnTreasure(const FChaosBreakEvent& BreakEvent);

	bool bBroken = false;
};
