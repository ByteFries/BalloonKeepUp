// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BalloonRelayGameMode.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ERelayGamePhase : uint8
{
	Waiting,
	Countdown,
	Playing,
	Result,
	GameOver
};

UCLASS()
class BALLOONKEEPUP_API ABalloonRelayGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	void NotifyBalloonHit(APlayerState* PlayerState);
	
	
private:
	void ChangePhase(ERelayGamePhase NewPhase);

	void Init();

	void EnterWaitingPhase();
	bool ArePlayersReady() const;
	void TryStartGame();

	void EnterCountdownPhase();
	void TickCountdown();
	void FinishCountdown();
	void BroadcastCountdown();
	
	void EnterPlayingPhase();

	void EnterResultPhase();

	void EnterGameOverPhase();

	

	void SendResultToInstance();

	void IncreaseRelayCount();

	//virtual void PostLogin(APlayerController* NewPlayer) override;
	

	
	FTimerHandle CountdownHandle;
	int CountdownRemaining = 3;

	FTimerHandle WaitingCheckHandle;
	
	ERelayGamePhase GamePhase = ERelayGamePhase::Waiting;

	TObjectPtr<APlayerState> LastHitPlayer;
	int RelayCount = 0;
	
};
