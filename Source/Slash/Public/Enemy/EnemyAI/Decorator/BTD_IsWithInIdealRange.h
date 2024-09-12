#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsWithInIdealRange.generated.h"

UCLASS()
class SLASH_API UBTD_IsWithInIdealRange : public UBTDecorator
{
public:
	GENERATED_BODY()
	UBTD_IsWithInIdealRange();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector AttackTargetKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector IdealRangeKey;
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const;

	float ErrorMarin = 30.f;
};