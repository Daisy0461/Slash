// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParryInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UParryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IParryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	bool isParryDilation = false;
	virtual bool ParryCheck();
	virtual void RestoreParryTimeDilation() = 0;
	/*
	ParrySystem의 문제점이
	Playr가 Parry에 성공하면 World는 0.2가 된다.
	그리고 OnParryBox가 없는 Weapon을 소지한 Player의 CustomTimeDilation은 0.5가 된다.
	이때 Weapon 내부에서 bool을 돌려도 Player의 Weapon과 Enemy의 weapon은 다르기에 당연히 값이 갱신이 안된다.
	그렇다면 이미 include한 Interface에서 함수를 만들어 사용하는 것이 제일 낫다고 판단된다.
	*/
	virtual void SetIsParryDilation(bool ParryDilation) = 0;
	virtual bool GetIsParryDilation();
private:
	
};
