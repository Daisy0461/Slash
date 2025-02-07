// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelStreamerActor.generated.h"

class UBoxComponent;

UCLASS()
class SLASH_API ALevelStreamerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelStreamerActor();
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void DelayedLoadLevel();
	ACharacter* Player;
	FVector OverlapVolumeLocation;

	//UnLoad Delay
	UPROPERTY(EditAnywhere)
	bool bUseUnLoadDelay = false;
	UPROPERTY(EditAnywhere)
	float UnLoadDelay = 1.0f;
	FTimerHandle UnloadTimer;

	//UnLoad Distance
	UPROPERTY(EditAnywhere)
	bool bUseUnLoadDistance = false;
	bool bIsCheckingDistance = false;
	UPROPERTY(EditAnywhere)
	float UnLoadDistance = 1000.0f;

	UPROPERTY(EditAnywhere)
	FName LevelToLoad;
	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()	
	void OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void UnloadLevel();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* OverlapVolume;

};
