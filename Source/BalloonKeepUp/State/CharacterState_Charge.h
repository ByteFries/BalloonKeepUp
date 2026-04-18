// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State/CharacterState.h"
#include "CharacterState_Charge.generated.h"

/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API UCharacterState_Charge : public UCharacterState
{
	GENERATED_BODY()
public:
	virtual void EnterState_Server() override;
	virtual void EnterState_Client() override;
	virtual void ExitState_Server() override;
	virtual void ExitState_Client() override;
	virtual void Tick_Server(float DeltaTime) override;
	virtual void Tick_Client(float DeltaTime) override;
};
