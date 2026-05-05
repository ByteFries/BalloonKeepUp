// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Time/TimeStepSubscriber.h"
#include "BalloonRelayGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRelayGameMode, Log, All);
/**
 * 
 */
UENUM(BlueprintType)
enum class ERelayGamePhase : uint8
{
	None,
	Waiting,
	Countdown,
	Playing,
	Result,
	GameOver
};

UCLASS()
class BALLOONKEEPUP_API ABalloonRelayGameMode : public AGameModeBase, public ITimeStepSubscriber
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void NotifyBalloonHit(APlayerState* PlayerState);
	
	virtual void OnFixedStep_Implementation(float FixedDeltaTime) override;
private:	
	void ChangePhase(ERelayGamePhase NewPhase);

	void Init();

	void EnterWaitingPhase();
	bool ArePlayersReady() const;
	void TryStartGame();
	
	void EnterCountdownPhase();
	void TickCountdownPhase(float DeltaTime);
	void FinishCountdown();
	
	void EnterPlayingPhase();

	void EnterResultPhase();

	void EnterGameOverPhase();

	void SendResultToInstance();

	void IncreaseRelayCount();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	static FString PhaseToString(ERelayGamePhase Phase);
	
	float CountdownTimeRemaining = 3.f;
	int LastBroadcastSecond = -1;

	FTimerHandle WaitingCheckHandle;
	
	ERelayGamePhase GamePhase = ERelayGamePhase::None;

	TObjectPtr<APlayerState> LastHitPlayer;
	int RelayCount = 0;
	
};

