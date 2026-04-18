// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CharacterState.generated.h"

/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API UCharacterState : public UObject
{
	GENERATED_BODY()
public:
	virtual void EnterState_Server() {}
	virtual void ExitState_Server() {}
	virtual void EnterState_Client() {}
	virtual void ExitState_Client() {}
	virtual void Tick_Server(float DeltaTime) {}
	virtual void Tick_Client(float DeltaTime) {}
};
