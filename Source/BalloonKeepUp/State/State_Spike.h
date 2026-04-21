// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State/StateBase.h"
#include "State_Spike.generated.h"

/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API UState_Spike : public UStateBase
{
	GENERATED_BODY()
public:
	virtual bool Init(UStateMachineComponent* InMachine, AActor* InOwner) override;
	virtual void Enter() override;
	virtual void Exit() override;

private:
	virtual void HandlePressed(const EInputAction Action) override;
	
	virtual void HandleReleased(const EInputAction Action) override;
	
	UPROPERTY()
	TObjectPtr<ABalloonKeepUpCharacter> OwnerCharacter;
};



