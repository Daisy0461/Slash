// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/VikingGameState.h"

void AVikingGameState::AddAttack(FAttackInfo AttackInfo)
{
    ActiveAttacks.AddUnique(AttackInfo);
}

void AVikingGameState::RemoveAttack(FAttackInfo AttackInfo)
{
    ActiveAttacks.Remove(AttackInfo);
}