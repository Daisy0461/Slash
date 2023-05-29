// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UAnimMontage;

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit(const FVector& ImpactPoint) override;

private:

	//Montage
	void Play_Warrior_HitReact_Montage(const FName& SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint);

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Sounds" )
	USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "VisualEffects")
	UParticleSystem* HitParticles;
};
