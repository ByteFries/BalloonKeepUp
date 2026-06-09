// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/BalloonKeepUpPlayerState.h"

#include "Net/UnrealNetwork.h"

ABalloonKeepUpPlayerState::ABalloonKeepUpPlayerState()
{
	bReplicates = true;
}

void ABalloonKeepUpPlayerState::SetReady(bool Ready)
{
	if (HasAuthority())
	{
		bReady = Ready;
	}
}

void ABalloonKeepUpPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABalloonKeepUpPlayerState, bReady);
}
