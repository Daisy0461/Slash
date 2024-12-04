// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyAOEBase.generated.h"

class USphereComponent;

UCLASS()
class SLASH_API AEnemyAOEBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyAOEBase();
	virtual void Tick(float DeltaTime) override; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnAOESphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);	
	UPROPERTY(EditAnywhere, Category = "AOE")
	bool DrawAOESphere = false;
	float DrawTime = 2.f;
private:
	UPROPERTY(EditAnywhere, Category = "AOE")
	USphereComponent* SphereComp;

};
