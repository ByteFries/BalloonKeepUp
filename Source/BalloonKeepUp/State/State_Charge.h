// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State/StateBase.h"
#include "State_Charge.generated.h"

/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API UState_Charge : public UStateBase
{
	GENERATED_BODY()
public:
	virtual bool Init(UStateMachineComponent* InMachine, AActor* InOwner) override;

	virtual void Enter() override;
	
	virtual void Exit() override;

private:
	virtual void HandleTriggered(const EInputAction Action) override;
	
	virtual void HandlePressed(const EInputAction Action) override;
	
	virtual void HandleReleased(const EInputAction Action) override;
	
	void Commit(const EInputAction Action);
	
	virtual void Cancel() override;
	
	TObjectPtr<ABalloonKeepUpCharacter> OwnerCharacter;
	
	float ChargeRatio = 0.f;
	float MaxChargeTime = 1.f;
	float ChargeStartTime = 0.f;
};
