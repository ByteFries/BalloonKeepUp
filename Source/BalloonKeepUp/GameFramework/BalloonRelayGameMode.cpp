// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/BalloonRelayGameMode.h"

#include "BalloonRelayGameState.h"

void ABalloonRelayGameMode::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void ABalloonRelayGameMode::NotifyBalloonHit(APlayerState* PlayerState)
{
	if (!PlayerState) return;
	if (GamePhase != ERelayGamePhase::Playing) return;
	
	if (PlayerState == LastHitPlayer.Get())
	{
		ChangePhase(ERelayGamePhase::GameOver);
		return;
	}
	
	LastHitPlayer = PlayerState;
	IncreaseRelayCount();
}

void ABalloonRelayGameMode::ChangePhase(ERelayGamePhase NewPhase)
{
	if (NewPhase == GamePhase) return;
	GamePhase = NewPhase;
	
	switch (GamePhase)
	{
	case ERelayGamePhase::Waiting:
		EnterWaitingPhase();
		break;
	case ERelayGamePhase::Countdown:
		EnterCountdownPhase();
		break;
	case ERelayGamePhase::Playing:
		EnterPlayingPhase();
		break;
	case ERelayGamePhase::Result:
		EnterResultPhase();
		break;
	case ERelayGamePhase::GameOver:
		EnterGameOverPhase();
		break;
	}
}

void ABalloonRelayGameMode::Init()
{
	
}

void ABalloonRelayGameMode::EnterWaitingPhase()
{
	GetWorldTimerManager().SetTimer(WaitingCheckHandle, this, &ABalloonRelayGameMode::TryStartGame, 0.2f, true);
}

bool ABalloonRelayGameMode::ArePlayersReady() const
{
	int ReadyCount = 0;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC) continue;

		if (PC->PlayerState && PC->GetPawn())
		{
			ReadyCount++;
		}
	}

	return ReadyCount >= 2;
}
/*
void ABalloonRelayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	TryStartGame();
}
*/

void ABalloonRelayGameMode::TryStartGame()
{
	if (ArePlayersReady())
	{
		ChangePhase(ERelayGamePhase::Countdown);
	}
}

void ABalloonRelayGameMode::EnterCountdownPhase()
{
	// 게임 시작 카운트 다운
	CountdownRemaining = 3;
	BroadcastCountdown();
	GetWorldTimerManager().SetTimer(CountdownHandle, this, &ABalloonRelayGameMode::TickCountdown, 1.f, true);
}

void ABalloonRelayGameMode::TickCountdown()
{
	CountdownRemaining--;

	BroadcastCountdown();
	
	if (CountdownRemaining <= 0)
	{
		FinishCountdown();
		return;
	}
}

void ABalloonRelayGameMode::FinishCountdown()
{
	// 카운트 다운 끝나고 호출할 함수?
	GetWorldTimerManager().ClearTimer(CountdownHandle);
	ChangePhase(ERelayGamePhase::Playing);
}

void ABalloonRelayGameMode::BroadcastCountdown()
{
	ABalloonRelayGameState* GS = GetGameState<ABalloonRelayGameState>();

	if (!GS) return;

	GS->SetCountdownValue(CountdownRemaining);
}

void ABalloonRelayGameMode::EnterPlayingPhase()
{
	// 커스텀 시뮬 활성화, 플레이어 움직이기 가능 -> 인게임 시간이 움직이기 시작
	
}

void ABalloonRelayGameMode::EnterResultPhase()
{
	// 
}

void ABalloonRelayGameMode::EnterGameOverPhase()
{
	// SendResultToInstance 호출?
}




void ABalloonRelayGameMode::SendResultToInstance()
{
	// 인스턴스에게 승/패 같은 메시지 보냄
}

void ABalloonRelayGameMode::IncreaseRelayCount()
{
	// 릴레이 카운트 증가 후 브로드캐스트
}



