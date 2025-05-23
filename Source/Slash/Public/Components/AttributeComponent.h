// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FORCEINLINE float GetFirstSkillCost() {return FirstSkillCost;}
	FORCEINLINE float GetSecondSkillCost() {return SecondSkillCost;}
	FORCEINLINE float GetThirdSkillCost() {return ThirdSkillCost;}
	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);
	float GetHealthPercent();
	float GetStaminaPercent();

	void Heal(float HealAmount);
	bool IsAlive();

	FORCEINLINE int32 GetTreasure() const {return Treasure;};
	FORCEINLINE float GetDodgeCost() const {return DodgeCost;};
	FORCEINLINE float GetStamina() const {return Stamina;};
	FORCEINLINE float GetGuardCost() const {return GuardCost;};
	void AddTreasure(int32 NumberOfTreasure);
	void SetStamina(float SetStamina);
	void StaminaRegen(float DeltaTime);


protected:
	virtual void BeginPlay() override;

private:
	//Health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	//Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina = 100.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina = 100.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost= 5.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float GuardCost= 0.05f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float FirstSkillCost= 18.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float SecondSkillCost= 15.f;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float ThirdSkillCost= 12.f;
	

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Treasure;
};
