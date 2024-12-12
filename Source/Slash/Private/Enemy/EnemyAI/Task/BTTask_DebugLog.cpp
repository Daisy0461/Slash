// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/EnemyAI/Task/BTTask_DebugLog.h"

UBTTask_DebugLog::UBTTask_DebugLog()
{

}

EBTNodeResult::Type UBTTask_DebugLog::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (!LogMessage.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("DebugLog: %s"), *LogMessage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DebugLog: No message provided."));
	}

	// Return success
	return EBTNodeResult::Succeeded;
}

FString UBTTask_DebugLog::GetStaticDescription() const
{
    if(!LogMessage.IsEmpty()) return FString::Printf(TEXT("\nLog: %s"), *LogMessage);
    else return FString::Printf(TEXT("\nLog is null"));
}