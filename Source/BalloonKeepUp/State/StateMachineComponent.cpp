// Fill out your copyright notice in the Description page of Project Settings.


#include "State/StateMachineComponent.h"

#include "StateBase.h"

// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	RequestTransition(DefaultStateClass);
}

bool UStateMachineComponent::RequestTransition(const TSubclassOf<UStateBase>& NewStateClass, FName Reason)
{
	if (!NewStateClass || !CanTransition(CurrentState, NewStateClass)) return false;

	ChangeState_Internal(NewStateClass);
	return true;
}

void UStateMachineComponent::HandleInput(const EInputAction Action, const ETriggerEvent Event)
{
	if (CurrentState) CurrentState->HandleInput(Action, Event);
}



void UStateMachineComponent::ChangeState_Internal(TSubclassOf<UStateBase> NextStateClass)
{
	if (CurrentState)
	{
		CurrentState->Exit();
		CurrentState = nullptr;
	}

	CurrentState = NewObject<UStateBase>(this, NextStateClass);

	if (!CurrentState) return;

	CurrentState->Init(this, GetOwner());
	CurrentState->Enter();
}

bool UStateMachineComponent::CanTransition(UStateBase* From, TSubclassOf<UStateBase> To)
{
	if (!To) return false;

	//if (From && From->IsA(U))

	
	return true;
}

void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentState) CurrentState->Tick(DeltaTime);
}