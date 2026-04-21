// Fill out your copyright notice in the Description page of Project Settings.


#include "State/State_Spike.h"

#include "BalloonKeepUpCharacter.h"
#include "StateMachineComponent.h"
#include "State_Idle.h"

bool UState_Spike::Init(UStateMachineComponent* InMachine, AActor* InOwner)
{
	if (!InMachine || !InOwner) return false;

	OwnerActor = InOwner;
	
	ABalloonKeepUpCharacter* Character = Cast<ABalloonKeepUpCharacter>(OwnerActor);
	
	if (!Character) return false;
	
	OwnerCharacter = Character;
	
	Machine = InMachine;

	return true;
}

void UState_Spike::Enter()
{
	if (!OwnerCharacter) return;
	OwnerCharacter->DoSpike();
}

void UState_Spike::Exit()
{
}

void UState_Spike::HandlePressed(const EInputAction Action)
{
	switch (Action)
	{
	case EInputAction::Jump:
		
		break;
	default:
		break;
	}
}

void UState_Spike::HandleReleased(const EInputAction Action)
{
	switch (Action)
	{
	case EInputAction::Jump:
		
		break;
	default:
		break;	
	}
}
