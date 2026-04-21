// Fill out your copyright notice in the Description page of Project Settings.


#include "State/State_Idle.h"

#include "StateMachineComponent.h"
#include "State_Charge.h"
#include "State_Dive.h"


void UState_Idle::Enter()
{
	Super::Enter();
}

void UState_Idle::Exit()
{
	Super::Exit();
}

void UState_Idle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UState_Idle::HandlePressed(const EInputAction Action)
{
	switch (Action)
	{
	case EInputAction::Receive:
		Machine->RequestTransition(UState_Charge::StaticClass());
		break;
	case EInputAction::Spike:
		Machine->RequestTransition(UState_Charge::StaticClass());
		break;
	case EInputAction::Jump:
		break;
	case EInputAction::Dive:
		Machine->RequestTransition(UState_Dive::StaticClass());
		break;
	default:
		break;
	}
}

void UState_Idle::HandleTriggered(const EInputAction Action)
{
	switch (Action)
	{
	case EInputAction::Jump:
		break;
	default:
		break;
	}
}

void UState_Idle::HandleReleased(const EInputAction Action)
{
	switch (Action)
	{
	case EInputAction::Jump:
		break;
	default:
		break;
	}
}

