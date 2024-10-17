// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "GameFramework/Actor.h"
#include "Arrow.generated.h"

class UBoxComponent;

UCLASS()
class SLASH_API AArrow : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArrow();
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UBoxComponent* ArrowBox;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ArrowMesh;


};
