
#include "Level/LevelStreamerActor.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ALevelStreamerActor::ALevelStreamerActor()
{
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;

	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelStreamerActor::OverlapBegins);
	OverlapVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &ALevelStreamerActor::OverlapEnds);
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevelStreamerActor::BeginPlay()
{
    Super::BeginPlay();

    Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (!Player){
		UE_LOG(LogTemp, Error, TEXT("Player not found"));
		return;
	}

    FVector PlayerLocation = Player->GetActorLocation();
    OverlapVolumeLocation = OverlapVolume->GetComponentLocation();
    float Distance = FVector::Dist(PlayerLocation, OverlapVolumeLocation);
	//UE_LOG(LogTemp, Display, TEXT("Distance: %f"), Distance);

	FVector Extent = OverlapVolume->GetScaledBoxExtent();
    float Diagonal = Extent.Size();  // 대각선 길이 계산
	//UE_LOG(LogTemp, Display, TEXT("OverlapVolume Diagonal Length: %f"), Diagonal);	
    if (LevelToLoad != "" && Distance < Diagonal)
    {
        FTimerHandle LoadTimer;
        GetWorldTimerManager().SetTimer(LoadTimer, this, &ALevelStreamerActor::LoadLevel, 0.1f, false);
    }
}

void ALevelStreamerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bUseUnLoadDistance && bIsCheckingDistance){
		FVector PlayerLocation = Player->GetActorLocation();
		OverlapVolumeLocation = OverlapVolume->GetComponentLocation();
		float Distance = FVector::Dist(PlayerLocation, OverlapVolumeLocation);

		if(Distance > UnLoadDistance){
			bIsCheckingDistance = false;
			if(bUseUnLoadDelay){
				GetWorldTimerManager().SetTimer(UnloadTimer, this, &ALevelStreamerActor::UnloadLevel, UnLoadDelay, false);
			}else{
				UnloadLevel();
			}
		}
	}
}

void ALevelStreamerActor::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	ACharacter* OverlapedCharacter = Cast<ACharacter>(OtherActor);

	if(OverlapedCharacter && Player == OverlapedCharacter && LevelToLoad != "")
	{
		LoadLevel();
	}
}

void ALevelStreamerActor::OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	ACharacter* OverlapedCharacter = Cast<ACharacter>(OtherActor);

	if (Player == OverlapedCharacter && LevelToLoad != "")
	{
		if(bUseUnLoadDistance){
			bIsCheckingDistance = true;
		}else if(bUseUnLoadDelay){
			GetWorldTimerManager().SetTimer(UnloadTimer, this, &ALevelStreamerActor::UnloadLevel, UnLoadDelay, false);
		}else{
			UnloadLevel();
		}
	}
}

void ALevelStreamerActor::LoadLevel()
{
    FLatentActionInfo LatentInfo;
    UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);
}

void ALevelStreamerActor::UnloadLevel()
{
	FLatentActionInfo LatentInfo;
	UGameplayStatics::UnloadStreamLevel(this, LevelToLoad, LatentInfo, false);
}