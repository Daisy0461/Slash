// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UWorld* world;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SineParameters")
	float Amplitude = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SineParameters")
	float TimeConstant = 5.f;

private: 
	float RunningTime; 
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UFUNCTION(BlueprintPure)
	float TransformSin();

	UFUNCTION(BlueprintPure)
	float TransformCos();

	
protected:
	/*해당 파라미터들은 PrimitiveComponent.h에서 찾았으며 원본은
	//DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_SixParams( FComponentBeginOverlapSignature, UPrimitiveComponent, OnComponentBeginOverlap, UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex, bool, bFromSweep, const FHitResult &, SweepResult);
	처럼 나와있다. 여기서 FComponent...는 FComponent...라는 Type을 선언하는 것이고 UPri..는 이 Delegate가 UPri..와 연관되어있다는 의이미여 
	다음 OnCompo...는 이 Delegate의 이름을 나타낸다.
	아래 SphereOverlap을 보면 간단하게 위에서 설명한 3개를 제외하고 ,를 지운 형태이다. */
	UFUNCTION()		//특정 Delegate는 dynamic multicast delegate라서 UFUNCTION을 추가해야한다. 이것이 없으면 AddDynamic으로 Delegate와 Bound(연결)되지 않는다.
	virtual void SphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
