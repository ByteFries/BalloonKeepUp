// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BalloonRelayGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountdownChanged, int, NewValue);

UCLASS()
class BALLOONKEEPUP_API ABalloonRelayGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	void SetCountdownValue(int NewValue);

	UPROPERTY(BlueprintAssignable)
	FOnCountdownChanged OnCountdownChanged;
protected:
	UFUNCTION()
	void OnRep_Countdown();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_Countdown)
	int CountdownValue = 0;
};
