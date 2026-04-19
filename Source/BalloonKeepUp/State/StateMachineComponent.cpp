// Fill out your copyright notice in the Description page of Project Settings.


#include "State/StateMachineComponent.h"

#include "StateBase.h"

// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	RequestTransition(DefaultStateClass);
}

bool UStateMachineComponent::RequestTransition(const TSubclassOf<UStateBase>& NewStateClass, FName Reason)
{
	if (!CanTransition(CurrentState, NewStateClass)) return false;
	if (CurrentState)
	{
		CurrentState->Exit();
		CurrentState = nullptr;
	}
	CurrentState = NewObject<UStateBase>(this, NewStateClass);
	if (!CurrentState) return false;
	
	CurrentState->Init(this, GetOwner());
	CurrentState->Enter();
	
	return true;
}

void UStateMachineComponent::HandleInput(EInputAction Action, EInputEvent Event)
{
	if (CurrentState) CurrentState->HandleInput(Action, Event);
}

bool UStateMachineComponent::CanTransition(UStateBase* From, TSubclassOf<UStateBase> To)
{
	return true;
}

void UStateMachineComponent::ChangeState_Internal(TSubclassOf<UStateBase> NextStateClass)
{
	
}


// Called every frame
void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentState) CurrentState->Tick(DeltaTime);
}