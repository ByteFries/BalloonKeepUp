// Fill out your copyright notice in the Description page of Project Settings.


#include "State/State_Dive.h"

#include "BalloonKeepUpCharacter.h"
#include "DiveStateOwner.h"
#include "StateMachineComponent.h"
#include "State_Idle.h"
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
	FVector Dir = OwnerActor->GetVelocity();
	Dir.Z = 0.f;

	if (!Dir.IsNearlyZero())
	{
		Dir.Normalize();
	}
	else
	{
		Dir = OwnerActor->GetActorForwardVector();
	}

	FVector DiveVel = Dir * DivePower;
	DiveVel.Z = DiveZ;
	
	DiveOwner->DoDive(DiveVel);
}

void UState_Dive::Exit()
{
}

void UState_Dive::Cancel()
{
	if (!DiveOwner) return;

	DiveOwner->CancelDive();
}
