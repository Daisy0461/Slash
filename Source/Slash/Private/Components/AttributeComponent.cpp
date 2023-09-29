// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"
#include "NiagaraFunctionLibrary.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health-Damage, 0.f, MaxHealth);
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina-StaminaCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent(){
	return Health/MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
    return Stamina / MaxStamina;
}
void UAttributeComponent::Heal(float HealAmount)
{
	Health = Health + HealAmount;
}

bool UAttributeComponent::IsAlive()
{
    return Health > 0.f;		//true = alive
}

void UAttributeComponent::AddTreasure(int32 NumberOfTreasure)
{
	Treasure += NumberOfTreasure;
}
void UAttributeComponent::SetStamina(float SetStamina)
{
	Stamina = SetStamina;
}

void UAttributeComponent::StaminaRegen(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}