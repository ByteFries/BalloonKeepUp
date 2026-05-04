// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/BalloonRelayGameState.h"

#include "Net/UnrealNetwork.h"

void ABalloonRelayGameState::SetCountdownValue(int NewValue)
{
	if (!HasAuthority()) return;

	CountdownValue = NewValue;

	OnCountdownChanged.Broadcast(CountdownValue);
}

void ABalloonRelayGameState::OnRep_Countdown()
{
	OnCountdownChanged.Broadcast(CountdownValue);
}

void ABalloonRelayGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABalloonRelayGameState, CountdownValue);
}
