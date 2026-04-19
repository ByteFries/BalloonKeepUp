// Fill out your copyright notice in the Description page of Project Settings.


#include "State/State_Dive.h"

#include "BalloonKeepUpCharacter.h"
#include "DiveStateOwner.h"
#include "Components/CapsuleComponent.h"
#include "Physics/Impulse/ImpulseBoxComponent.h"


bool UState_Dive::Init(UStateMachineComponent* InMachine, AActor* InOwner)
{
	if (!InMachine || !InOwner) return false;

	Machine = InMachine;

	OwnerActor = InOwner;

	IDiveStateOwner* Owner = Cast<IDiveStateOwner>(OwnerActor);

	if (!Owner) return false;

	DiveOwner = Owner;

	return true;
}

void UState_Dive::Enter()
{
	if (!DiveOwner) return;

	DiveOwner->RequestDive();
}

void UState_Dive::Exit()
{
	if (!DiveOwner) return;

	DiveOwner->EndDive();
	//OwnerCharacter->GetDiveBox()->Deactivate();
	//OwnerCharacter->GetCapsuleComponent()->SetCapsuleHalfHeight(90, true);	
}

void UState_Dive::Cancel()
{
	if (!DiveOwner) return;

	DiveOwner->CancelDive();
}

