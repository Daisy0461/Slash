// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "FloatingPlaform.generated.h"

UCLASS()
class SLASH_API AFloatingPlaform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlaform();

	UFUNCTION()
	void ReverseFloating();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
    void TimelineUpdate(float Value);
	void DestroyPlatform();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* FloatingMesh;
	FTimeline FloatingTimeline;
	UPROPERTY(EditAnywhere, Category = "Timeline")
    UCurveFloat* FloatingCurve;

	FVector InitialPosition;
	FVector NewPosition;
	float InitialZValue;
	float floatingHeight = 350.f;
	FTimerHandle DestroyTimerHandle;

	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
