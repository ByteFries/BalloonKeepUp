// Fill out your copyright notice in the Description page of Project Settings.


#include "State/State_Receive.h"

#include "BalloonKeepUpCharacter.h"
#include "StateMachineComponent.h"
#include "State_Idle.h"

bool UState_Receive::Init(UStateMachineComponent* InMachine, AActor* InOwner)
{
	if (!InMachine || !InOwner) return false;

	OwnerActor = InOwner;
	
	ABalloonKeepUpCharacter* Character = Cast<ABalloonKeepUpCharacter>(OwnerActor);
	
	if (!Character) return false;
	
	OwnerCharacter = Character;
	
	Machine = InMachine;

	return true;
}

void UState_Receive::Enter()
{
	OwnerCharacter->DoReceive();
}

void UState_Receive::Exit()
{
}

void UState_Receive::HandlePressed(const EInputAction Action)
{
}

void UState_Receive::HandleReleased(const EInputAction Action)
{
}
