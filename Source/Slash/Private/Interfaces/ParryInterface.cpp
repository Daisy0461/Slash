// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/ParryInterface.h"

// Add default functionality here for any IParryInterface functions that are not pure virtual.
bool IParryInterface::ParryCheck()
{
    return false;
}

bool IParryInterface::GetIsParryDilation()
{
    return isParryDilation;
}