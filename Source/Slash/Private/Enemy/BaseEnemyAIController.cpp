// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/BaseEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
//#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
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
    BTComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
    
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

//OnPossess, OnUnPossess, OnPerceptionUpdated, GetPerceptionInfo

void ABaseEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    UE_LOG(LogTemp, Display, TEXT("OnPossess Call"));
    Enemy = Cast<AEnemy>(InPawn);
    if (!Enemy)
    {
        UE_LOG(LogTemp, Display, TEXT("AI Controller Cast Fail"));
        return;
    }
    Enemy->OnEnemyDeath.AddDynamic(this, &ABaseEnemyAIController::OnEnemyDied);

    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ABaseEnemyAIController::OnPerceptionUpdated);
    }
    
    //RunBehaviorTree(Enemy->GetBehaviorTree());

    BlackboardComponent = Enemy->GetBlackboardComponent();
    if (!BlackboardComponent)
    {
        UE_LOG(LogTemp, Display, TEXT("Blackboard Component cann't find"));
        return;
    }
    BlackboardAsset = Enemy->GetBlackboardComponent()->GetBlackboardAsset();
    if(!BlackboardAsset){
        UE_LOG(LogTemp, Display, TEXT("BlackboardAsset cann't find"));
    }

    UseBlackboard(BlackboardAsset, BlackboardComponent);
    RunBehaviorTree(Enemy->GetBehaviorTree());
    

    // if (UseBlackboard(BlackboardAsset, BlackboardComponent)){
    //     //UE_LOG(LogTemp, Warning, TEXT("RunBehavior Tree"));
    //     //RunBehaviorTree(Enemy->GetBehaviorTree());
    // }else{
    //     UE_LOG(LogTemp, Warning, TEXT("Can't run Behavior Tree"));
    // }
    
    SetEnemyState(EEnemyState::EES_Passive);
    FName AttackRadiusKeyName = TEXT("AttackRadius");
    FName DefendRadiusKeyName = TEXT("DefendRadius");
    //FBlackboard::FKey AttackRadiusKey = BlackboardComponent->GetKeyID(AttackRadiusKeyName);
    // 블랙보드 키 확인
    // if (BlackboardComponent->GetKeyType(AttackRadiusKey) != UBlackboardKeyType_Float::StaticClass())
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("AttackRadiusKeyName does not exist or is not a float."));
    // }
    BlackboardComponent->SetValueAsFloat(AttackRadiusKeyName, Enemy->GetAttackRadius());
    //UE_LOG(LogTemp, Display, TEXT("Attack Radius set to: %f"), Enemy->GetAttackRadius());
    BlackboardComponent->SetValueAsFloat(DefendRadiusKeyName, Enemy->GetDefendRadius());
    //UE_LOG(LogTemp, Display, TEXT("Defend Radius set to: %f"), Enemy->GetDefendRadius());

    
}

void ABaseEnemyAIController::OnUnPossess()
{
    Super::OnUnPossess();
}

void ABaseEnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    //UE_LOG(LogTemp, Display, TEXT("OnPerceptionUpdated called. Updated Actors count: %d"), UpdatedActors.Num());
    
    for (AActor* Actor : UpdatedActors)
    {
        GetPerceptionInfo(Actor);
    }
}

void ABaseEnemyAIController::GetPerceptionInfo(AActor* Actor)
{
    //UE_LOG(LogTemp, Display, TEXT("In Get Perception for Actor: %s"), *Actor->GetName());
    
    if (AIPerceptionComponent)
    {
        FActorPerceptionBlueprintInfo PerceptionInfo;
        AIPerceptionComponent->GetActorsPerception(Actor, PerceptionInfo);
        //UE_LOG(LogTemp, Warning, TEXT("Stimulus count: %d"), PerceptionInfo.LastSensedStimuli.Num());
        // 감지된 자극 정보를 확인
        for (const FAIStimulus& Stimulus : PerceptionInfo.LastSensedStimuli)
        {
            //UE_LOG(LogTemp, Warning, TEXT("Stimulus Age: %f"), Stimulus.GetAge());
            if (Stimulus.WasSuccessfullySensed())
            {
                //UE_LOG(LogTemp, Warning, TEXT("Actor %s was successfully Sensed."), *Actor->GetName());

                const FAISenseID SightID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
                const FAISenseID HearingID = UAISense::GetSenseID(UAISense_Hearing::StaticClass());
                const FAISenseID DamageID = UAISense::GetSenseID(UAISense_Damage::StaticClass());

                if (Stimulus.Type == SightID)
                {
                    //UE_LOG(LogTemp, Warning, TEXT("Sensed by sight."));
                    SightSensed(Actor);
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
                //UE_LOG(LogTemp, Warning, TEXT("Actor %s Sensed was lost."), *Actor->GetName());
            }
        }
        //UE_LOG(LogTemp, Display, TEXT("Current Enemy State: %s"), *GetEnemyStateAsString(EnemyState));
    }
}

void ABaseEnemyAIController::OnEnemyDied()
{
    // Behavior Tree 중단
    UBehaviorTree* CurrentBehaviorTree = BTComp->GetCurrentTree();
    if (CurrentBehaviorTree)
    {
        UE_LOG(LogTemp, Warning, TEXT("Current Behavior Tree: %s"), *CurrentBehaviorTree->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No Behavior Tree is currently running."));
    }

    // if(BrainComponent == BTComp){
    //     UE_LOG(LogTemp, Display, TEXT("Braind & BTComp is Same"));
    // }else{
    //     UE_LOG(LogTemp, Display, TEXT("Braind & BTComp is not Same"));
    // }

    if (BTComp && BTComp->GetCurrentTree() == Enemy->GetBehaviorTree())
    {
        SetEnemyStateAsDead();
        BTComp->StopTree(EBTStopMode::Forced);
        UE_LOG(LogTemp, Warning, TEXT("Behavior Tree has been stopped due to enemy death!"));
    }else{
        UE_LOG(LogTemp, Display, TEXT("Not Same"));
    }
}

void ABaseEnemyAIController::SetEnemyState(const EEnemyState State)
{
    if (!BlackboardComponent)
    {
        UE_LOG(LogTemp, Display, TEXT("BlackboardComponent not found"));
        return;
    }
    //UE_LOG(LogTemp, Display, TEXT("In Set State"));
    uint8 StateValue = static_cast<uint8>(State);
    //UE_LOG(LogTemp, Display, TEXT("Setting Enemy State: %d"), StateValue);

    BlackboardComponent->SetValueAsEnum(StateKeyName, StateValue);
    EnemyState = State;
}

EEnemyState ABaseEnemyAIController::GetEnemyState() const
{
    return static_cast<EEnemyState>(BlackboardComponent->GetValueAsEnum(StateKeyName));
}

void ABaseEnemyAIController::SightSensed(AActor* AttackTarget)
{
    SetEnemyStateAsAttacking(AttackTarget);
}

void ABaseEnemyAIController::SetEnemyStateAsPassive()
{
    SetEnemyState(EEnemyState::EES_Passive);
}

void ABaseEnemyAIController::SetEnemyStateAsInvesting(const FVector InvestingLocation)
{
    if(!BlackboardComponent) {
        UE_LOG(LogTemp, Display, TEXT("In SetEnemyStateAsInvesting BlackComponent can't find"));
        return;
    }
    SetEnemyState(EEnemyState::EES_Investing);
    BlackboardComponent->SetValueAsVector(PointOfInterestKeyName, InvestingLocation);
}

void ABaseEnemyAIController::SetEnemyStateAsChasing()
{
    SetEnemyState(EEnemyState::EES_Chasing);
}

void ABaseEnemyAIController::SetEnemyStateAsStrafing()
{
    SetEnemyState(EEnemyState::EES_Strafing);
}

void ABaseEnemyAIController::SetEnemyStateAsAttacking(AActor* AttackTarget)
{
    if(!BlackboardComponent) {
        UE_LOG(LogTemp, Display, TEXT("In SetEnemyStateAsAttacking BlackComponent can't find"));
        return;
    }
    BlackboardComponent->SetValueAsObject(AttackTargetKeyName, AttackTarget);
    AttackTargetActor = AttackTarget;
    SetEnemyState(EEnemyState::EES_Attacking);
}

void ABaseEnemyAIController::SetEnemyStateAsParried()
{
    SetEnemyState(EEnemyState::EES_Parried);
}

void ABaseEnemyAIController::SetEnemyStateAsHitting()
{
    SetEnemyState(EEnemyState::EES_Hitting);
}

void ABaseEnemyAIController::SetEnemyStateAsDead()
{
    SetEnemyState(EEnemyState::EES_Dead);
    Enemy->SetEnemyState(EEnemyState::EES_Dead);
    //StopTree(Enemy->GetBehaviorTree());
}

FString ABaseEnemyAIController::GetEnemyStateAsString(EEnemyState State)
{
    switch (State)
    {
        case EEnemyState::EES_Passive:
            return "Passive";
        case EEnemyState::EES_Investing:
            return "Investing";
        case EEnemyState::EES_Chasing:
            return "Chasing";
        case EEnemyState::EES_Strafing:
            return "Strafing";
        case EEnemyState::EES_Attacking:
            return "Attacking";
        case EEnemyState::EES_Parried:
            return "Parried";
        case EEnemyState::EES_Hitting:
            return "Hitting";
        case EEnemyState::EES_Dead:
            return "Dead";
        case EEnemyState::EES_NoState:
            return "No State";
        default:
            return "Unknown";
    }
}
