// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/BalloonRelayGameState.h"

#include "Balloon/Balloon.h"
#include "Net/UnrealNetwork.h"

void ABalloonRelayGameState::AddRelayCount(int Value)
{
	if (!HasAuthority()) return;
	
	RelayCount += Value;
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
	
	ApplyGameplayFreeze(!bPlayEnabled);
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
	ApplyGameplayFreeze(!bPlayEnabled);
}

void ABalloonRelayGameState::OnRep_Balloon()
{
}

void ABalloonRelayGameState::ApplyGameplayFreeze(bool bFreeze)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	UE_LOG(LogTemp, Warning, TEXT("ApplyGameplayFreeze called. Freeze=%d"), bFreeze);
	if (PC)
	{
		PC->SetIgnoreMoveInput(bFreeze);
		PC->SetIgnoreLookInput(bFreeze);
	}

	//for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	//{
	//	APlayerController* PC = It->Get();
	//	if (PC && PC->IsLocalController())
	//	{
	//		// 이 PC만 freeze/unfreeze
	//		PC->SetIgnoreMoveInput(false);
	//		PC->SetIgnoreLookInput(false);
	//	}
	//}

	if (Balloon)
	{
		Balloon->SetFreeze(bFreeze);
	}
}

void ABalloonRelayGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABalloonRelayGameState, CountdownValue);
	DOREPLIFETIME(ABalloonRelayGameState, RelayCount);
	DOREPLIFETIME(ABalloonRelayGameState, Balloon);
	DOREPLIFETIME(ABalloonRelayGameState, bPlayEnabled);
}
