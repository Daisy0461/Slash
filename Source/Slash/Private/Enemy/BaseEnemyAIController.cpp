// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/BaseEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Enemy/Enemy.h"

ABaseEnemyAIController::ABaseEnemyAIController()
{
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));
    
    // Sight Configuration
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISense_Sight"));
    SightConfig->SightRadius = 1500.0f;
    SightConfig->LoseSightRadius = 1800.0f;
    SightConfig->PeripheralVisionAngleDegrees = 60.0f;
    AIPerceptionComponent->ConfigureSense(*SightConfig);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;  // 필요 시 주석 해제
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true; // 필요 시 주석 해제

    // Hearing Configuration
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("AISense_Hearing"));
    HearingConfig->HearingRange = 2000.0f;
    AIPerceptionComponent->ConfigureSense(*HearingConfig);

    // Damage Configuration
    DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("AISense_Damage"));
    AIPerceptionComponent->ConfigureSense(*DamageConfig);
}

void ABaseEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    if (AIPerceptionComponent)
    {
        UE_LOG(LogTemp, Display, TEXT("AIPerceptionComponent is initialized."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AIPerceptionComponent is NOT initialized!"));
    }
}

void ABaseEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    Enemy = Cast<AEnemy>(InPawn);
    
    if (!Enemy)
    {
        UE_LOG(LogTemp, Display, TEXT("AI Controller Cast Fail"));
        return;
    }

    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ABaseEnemyAIController::OnPerceptionUpdated);
    }

    RunBehaviorTree(Enemy->GetBehaviorTree());

    BlackboardComponent = GetBlackboardComponent();
    if (!BlackboardComponent)
    {
        UE_LOG(LogTemp, Display, TEXT("Blackboard Component is null"));
        return;
    }
    
    SetEnemyState(EEnemyState::EES_Passive);
    FName AttackRadiusKeyName = TEXT("AttackTarget");
    BlackboardComponent->SetValueAsFloat(AttackRadiusKeyName, Enemy->GetAttackRadius());
    FName DefendRadiusKeyName = TEXT("DefendRadius");
    BlackboardComponent->SetValueAsFloat(DefendRadiusKeyName, Enemy->GetDefendRadius());

    UE_LOG(LogTemp, Display, TEXT("On PossessEnd"));
}

void ABaseEnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    UE_LOG(LogTemp, Display, TEXT("OnPerceptionUpdated called. Updated Actors count: %d"), UpdatedActors.Num());
    
    for (AActor* Actor : UpdatedActors)
    {
        GetPerceptionInfo(Actor);
    }
}

void ABaseEnemyAIController::GetPerceptionInfo(AActor* Actor)
{
    UE_LOG(LogTemp, Display, TEXT("In Get Perception for Actor: %s"), *Actor->GetName());
    
    if (AIPerceptionComponent)
    {
        FActorPerceptionBlueprintInfo PerceptionInfo;
        AIPerceptionComponent->GetActorsPerception(Actor, PerceptionInfo);
        UE_LOG(LogTemp, Warning, TEXT("Stimulus count: %d"), PerceptionInfo.LastSensedStimuli.Num());
        // 감지된 자극 정보를 확인
        for (const FAIStimulus& Stimulus : PerceptionInfo.LastSensedStimuli)
        {
            UE_LOG(LogTemp, Warning, TEXT("Stimulus Age: %f"), Stimulus.GetAge());
            if (Stimulus.WasSuccessfullySensed())
            {
                UE_LOG(LogTemp, Warning, TEXT("Actor %s was successfully Sensed."), *Actor->GetName());

                const FAISenseID SightID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
                const FAISenseID HearingID = UAISense::GetSenseID(UAISense_Hearing::StaticClass());
                const FAISenseID DamageID = UAISense::GetSenseID(UAISense_Damage::StaticClass());

                if (Stimulus.Type == SightID)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Sensed by sight."));
                }
                else if (Stimulus.Type == HearingID)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Sensed by hearing."));
                }
                else if (Stimulus.Type == DamageID)
                {
                    UE_LOG(LogTemp, Display, TEXT("Sensed by Damage."));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Actor %s Sensed was lost."), *Actor->GetName());
            }
        }
    }
}

void ABaseEnemyAIController::SetEnemyState(const EEnemyState State)
{
    if (!BlackboardComponent)
    {
        UE_LOG(LogTemp, Display, TEXT("BlackboardComponent not found"));
        return;
    }
    BlackboardComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(State));
    EnemyState = State;
}

EEnemyState ABaseEnemyAIController::GetEnemyState() const
{
    return static_cast<EEnemyState>(BlackboardComponent->GetValueAsEnum(StateKeyName));
}
