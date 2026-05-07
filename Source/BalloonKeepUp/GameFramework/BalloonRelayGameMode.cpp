// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/BalloonRelayGameMode.h"

#include "BalloonKeepUpCharacter.h"
#include "BalloonRelayGameState.h"
#include "Balloon/Balloon.h"
#include "Balloon/BalloonSpawnPoint.h"
#include "Balloon/PopTriggerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Time/TimeManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogRelayGameMode);

void ABalloonRelayGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UTimeManagerSubsystem* TimeManager = Cast<UTimeManagerSubsystem>(GetWorld()->GetSubsystem<UTimeManagerSubsystem>()))
	{
		TimeManager->Register(this);
	}
	
	Init();
}

void ABalloonRelayGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UTimeManagerSubsystem* TimeManager = Cast<UTimeManagerSubsystem>(GetWorld()->GetSubsystem<UTimeManagerSubsystem>()))
	{
		TimeManager->Unregister(this);
	}
}

void ABalloonRelayGameMode::HandleBalloonOverlap(ABalloon* Balloon, AActor* Actor)
{
	if (GamePhase != ERelayGamePhase::Playing) return;
	if (!Balloon || !Actor) return;
	
	if (Actor->FindComponentByClass<UPopTriggerComponent>())
	{
		PopBalloon();
		return;
	}
	
	if (ABalloonKeepUpCharacter* Character = Cast<ABalloonKeepUpCharacter>(Actor))
	{
		if (APlayerState* PS = Character->GetPlayerState())
		{
			if (PS != LastHitPlayerState)
			{
				LastHitPlayerState = PS;
				IncreaseRelayCount();
			}
			else
			{
				PopBalloon();
			}
		}
	}

}

void ABalloonRelayGameMode::OnFixedStep_Implementation(float FixedDeltaTime)
{
	switch (GamePhase)
	{
	case ERelayGamePhase::Countdown:
		TickCountdownPhase(FixedDeltaTime);
		break;
	default:
		break;
	}
}

void ABalloonRelayGameMode::ChangePhase(ERelayGamePhase NewPhase)
{
	if (NewPhase == GamePhase) return;
	UE_LOG(LogRelayGameMode, Warning, TEXT("[GameMode] Phase Exit: %s"), *PhaseToString(GamePhase));
	GamePhase = NewPhase;
	UE_LOG(LogRelayGameMode, Warning, TEXT("[GameMode] Phase Enter: %s"), *PhaseToString(GamePhase));

	
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
	default:
		break;
	}
}

void ABalloonRelayGameMode::Init()
{
	ChangePhase(ERelayGamePhase::Waiting);
}

void ABalloonRelayGameMode::EnterWaitingPhase()
{
	
	if (ABalloonRelayGameState* GS = GetGameState<ABalloonRelayGameState>()) GS->SetPlayEnabled(false);
	
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

void ABalloonRelayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	TryStartGame();
}


void ABalloonRelayGameMode::TryStartGame()
{
	if (ArePlayersReady())
	{
		ChangePhase(ERelayGamePhase::Countdown);
	}
}

void ABalloonRelayGameMode::EnterCountdownPhase()
{
	GetWorldTimerManager().ClearTimer(WaitingCheckHandle);
	
	// 게임 시작 카운트 다운
	ABalloonRelayGameState* GS = GetGameState<ABalloonRelayGameState>();
	if (GS) GS->SetPlayEnabled(false);
	CountdownTimeRemaining = 3.f;
	LastBroadcastSecond = -1;

	TickCountdownPhase(0.f);
}

void ABalloonRelayGameMode::TickCountdownPhase(float DeltaSecond)
{
	CountdownTimeRemaining -= DeltaSecond;
	CountdownTimeRemaining = FMath::Max(0.f, CountdownTimeRemaining);

	int CurrentSecond = FMath::CeilToInt(CountdownTimeRemaining);

	if (CurrentSecond != LastBroadcastSecond)
	{
		LastBroadcastSecond = CurrentSecond;
		ABalloonRelayGameState* GS = GetGameState<ABalloonRelayGameState>();
		if (GS)
		{
			GS->SetCountdownValue(CurrentSecond);

			if (CurrentSecond == 2)
			{
				SpawnBalloon();
			}
		}
	}
	
	if (CurrentSecond == 0)
	{
		FinishCountdown();
	}
}

void ABalloonRelayGameMode::SpawnBalloon()
{
	if (!BalloonClass) return;
	
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABalloonSpawnPoint::StaticClass(), SpawnPoints);

	if (SpawnPoints.Num() == 0) return;

	int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
	ABalloonSpawnPoint* SpawnPoint = Cast<ABalloonSpawnPoint>(SpawnPoints[Index]);
	if (!SpawnPoint) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABalloon* SpawnedBalloon = GetWorld()->SpawnActor<ABalloon>(
		BalloonClass,
		SpawnPoint->GetSpawnLocation(),
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!SpawnedBalloon) return;

	if (ABalloonRelayGameState* GS = GetGameState<ABalloonRelayGameState>())
	{
		GS->SetBalloon(SpawnedBalloon);
	}
}

void ABalloonRelayGameMode::FinishCountdown()
{
	ChangePhase(ERelayGamePhase::Playing);
}

void ABalloonRelayGameMode::EnterPlayingPhase()
{
	ABalloonRelayGameState* GS = GetGameState<ABalloonRelayGameState>();
	if (GS) GS->SetPlayEnabled(true);
}

void ABalloonRelayGameMode::EnterResultPhase()
{
	// 결과 ui 창 보여주기
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
	if (ABalloonRelayGameState* GS = GetGameState<ABalloonRelayGameState>())
	{
		GS->AddRelayCount(1);
	}
}

void ABalloonRelayGameMode::PopBalloon()
{
	if (ABalloonRelayGameState* GS = GetGameState<ABalloonRelayGameState>())
	{
		GS->GetBalloon()->PopBalloon();
	}
	
	ChangePhase(ERelayGamePhase::GameOver);
}

FString ABalloonRelayGameMode::PhaseToString(ERelayGamePhase Phase)
{
	const UEnum* EnumPtr = StaticEnum<ERelayGamePhase>();
	
	if (!EnumPtr) return TEXT("InvalidPhase");
	return EnumPtr->GetNameStringByValue((int64)Phase);
}



