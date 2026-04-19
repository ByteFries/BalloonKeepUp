// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateBase.h"
#include "State_Idle.generated.h"

/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API UState_Idle : public UStateBase
{
	GENERATED_BODY()
public:
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Tick(float DeltaTime) override;
};
