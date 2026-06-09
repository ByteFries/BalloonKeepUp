// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/BalloonRelayGameState.h"

#include "Balloon/Balloon.h"
#include "Net/UnrealNetwork.h"

void ABalloonRelayGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ABalloonRelayGameState::AddRelayCount(int Value)
{
	if (!HasAuthority()) return;
	
	RelayCount += Value;

	UE_LOG(LogTemp, Log, TEXT("[%s][Relay] RelayCount Changed: %d (Added: %d)"),
	HasAuthority() ? TEXT("Server") : TEXT("Client"),
	RelayCount,
	Value
	);
	OnRelayCountChanged.Broadcast(RelayCount);
}

void ABalloonRelayGameState::SetCountdownValue(int NewValue)
{
	if (!HasAuthority()) return;

	CountdownValue = NewValue;
	OnCountdownChanged.Broadcast(CountdownValue);
}

void ABalloonRelayGameState::SetPlayEnabled(bool NewValue)
{
	if (!HasAuthority()) return;
	if (bPlayEnabled == NewValue) return;
	
	bPlayEnabled = NewValue;
	
	OnPlayEnableChanged.Broadcast(bPlayEnabled);
}

void ABalloonRelayGameState::SetBalloon(ABalloon* InBalloon)
{
	if (InBalloon)
	{
		Balloon = InBalloon;
	}
}

void ABalloonRelayGameState::OnRep_Countdown()
{
	OnCountdownChanged.Broadcast(CountdownValue);
}

void ABalloonRelayGameState::OnRep_RelayCount()
{
	OnRelayCountChanged.Broadcast(RelayCount);
}

void ABalloonRelayGameState::OnRep_PlayEnabled()
{
	OnPlayEnableChanged.Broadcast(bPlayEnabled);
}

void ABalloonRelayGameState::OnRep_Balloon()
{
}

void ABalloonRelayGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABalloonRelayGameState, CountdownValue);
	DOREPLIFETIME(ABalloonRelayGameState, RelayCount);
	DOREPLIFETIME(ABalloonRelayGameState, Balloon);
	DOREPLIFETIME(ABalloonRelayGameState, bPlayEnabled);
}
