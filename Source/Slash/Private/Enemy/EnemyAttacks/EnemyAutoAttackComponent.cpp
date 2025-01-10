// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAttacks/EnemyAutoAttackComponent.h"

// Sets default values for this component's properties
UEnemyAutoAttackComponent::UEnemyAutoAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	OwnerActor = Cast<AActor>(GetOwner());
}

// Called when the game starts
void UEnemyAutoAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}


