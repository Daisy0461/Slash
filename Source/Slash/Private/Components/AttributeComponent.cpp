// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"
#include "NiagaraFunctionLibrary.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health-Damage, 0.f, MaxHealth);
}

float UAttributeComponent::GetHealthPercent(){
	return Health/MaxHealth;
}

void UAttributeComponent::Heal(float HealAmount)
{
	Health = Health + HealAmount;
}

bool UAttributeComponent::IsAlive()
{
    return Health > 0.f;		//true = alive
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}