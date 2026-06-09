// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Time/TimeStepSubscriber.h"
#include "BalloonRelayGameMode.generated.h"

class ABalloon;
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
	
	void HandleBalloonOverlap(ABalloon* Balloon, AActor* Actor);
	
	virtual void OnFixedStep_Implementation(float FixedDeltaTime) override;

	void CheckAllPlayersReady();
private:
	void ChangePhase(ERelayGamePhase NewPhase);

	void Init();

	void EnterWaitingPhase();
	bool AreAllPlayersReady() const;
	
	void EnterCountdownPhase();
	void TickCountdownPhase(float DeltaTime);
	void SpawnBalloon();
	void FinishCountdown();
	
	void EnterPlayingPhase();

	void EnterResultPhase();

	void EnterGameOverPhase();

	void SendResultToInstance();

	void IncreaseRelayCount();

	void PopBalloon();

	static FString PhaseToString(ERelayGamePhase Phase);
	
	float CountdownTimeRemaining = 3.f;
	int LastBroadcastSecond = -1;

	FTimerHandle WaitingCheckHandle;
	
	ERelayGamePhase GamePhase = ERelayGamePhase::None;

	int RelayCount = 0;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABalloon> BalloonClass;
	
	TObjectPtr<APlayerState> LastHitPlayerState;

	int ExpectedPlayerCount = 2;
};

