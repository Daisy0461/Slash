// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SliceActor.generated.h"

class UProceduralMeshComponent;
class UStaticMeshComponent;

UCLASS()
class SLASH_API ASliceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASliceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProcMeshComponent;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void CopyFromStaticMesh();

};
