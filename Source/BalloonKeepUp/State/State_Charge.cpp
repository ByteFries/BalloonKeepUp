// Fill out your copyright notice in the Description page of Project Settings.


#include "State/State_Charge.h"

#include "BalloonKeepUpCharacter.h"

bool UState_Charge::Init(UStateMachineComponent* InMachine, AActor* InOwner)
{
	if (!InMachine || !InOwner) return false;

	Machine = InMachine;
	OwnerActor = InOwner;
	
	ABalloonKeepUpCharacter* Character = Cast<ABalloonKeepUpCharacter>(InOwner);

	if (!Character) return false;

	OwnerCharacter = Character;

	return true;
}

void UState_Charge::Enter()
{
	if (!OwnerActor) return;
	ChargeStartTime = GetWorld()->GetTimeSeconds();
	ChargeRatio = 0;
}


void UState_Charge::Exit()
{
	ChargeStartTime = 0.f;
	ChargeRatio = 0.f;
}

void UState_Charge::HandleTriggered(const EInputAction Action)
{
	switch (Action)
	{
		case EInputAction::Jump:
			break;
		case EInputAction::Dive:
			Cancel();
			break;
		default:
			break;
	}
}

void UState_Charge::HandlePressed(const EInputAction Action)
{
	switch (Action)
	{
	case EInputAction::Jump:
		break;
	case EInputAction::Dive:
		Cancel();
	default:
		break;
	}
}

void UState_Charge::HandleReleased(const EInputAction Action)
{
	switch (Action)
	{
	case EInputAction::Jump:
		break;
	case EInputAction::Receive:
		OwnerCharacter->RequestChargeAction(Action, ChargeRatio);
		break;
	case EInputAction::Spike:
		OwnerCharacter->RequestChargeAction(Action, ChargeRatio);
		break;
	default:
		break;
	}
}

void UState_Charge::Commit()
{
	if (!OwnerActor) return;

	const float Elapsed = GetWorld()->GetTimeSeconds() - ChargeStartTime;
	
	ChargeRatio = FMath::Clamp(Elapsed / MaxChargeTime, 0.3f, 1.5f);

	//OwnerCharacter->RequestChargeAction(ChargeRatio);
}

void UState_Charge::Cancel()
{
	if (!OwnerActor) return;
	//OwnerCharacter->Requst
}