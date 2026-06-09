// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BalloonKeepUpPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API ABalloonKeepUpPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ABalloonKeepUpPlayerState();
	
	void SetReady(bool Ready);
	
	bool IsReady() const { return bReady; }

private:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	bool bReady = false;
};
