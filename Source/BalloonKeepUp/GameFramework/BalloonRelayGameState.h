// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BalloonRelayGameState.generated.h"

class ABalloon;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountdownChanged, int, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRelayCountChanged, int, NewValue);


UCLASS()
class BALLOONKEEPUP_API ABalloonRelayGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	void AddRelayCount(int Value);
	
	void SetCountdownValue(int NewValue);

	void SetPlayEnabled(bool NewValue);
	
	UPROPERTY()
	FOnCountdownChanged OnCountdownChanged;

	UPROPERTY()
	FOnRelayCountChanged OnRelayCountChanged;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Countdown();

	UFUNCTION()
	void OnRep_RelayCount();

	UFUNCTION()
	void OnRep_PlayEnabled();
	
	UFUNCTION()
	void OnRep_Balloon();

	void ApplyGameplayFreeze(bool bFreeze);
	
	UPROPERTY(ReplicatedUsing=OnRep_Countdown)
	int CountdownValue = 0;

	UPROPERTY(ReplicatedUsing=OnRep_RelayCount)
	int RelayCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_PlayEnabled)
	bool bPlayEnabled = true;

	UPROPERTY(ReplicatedUsing=OnRep_Balloon)
	TObjectPtr<ABalloon> Balloon;
};

