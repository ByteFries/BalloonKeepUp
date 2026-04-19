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

	virtual void Commit() override;
	virtual void Cancel() override;

private:
	TObjectPtr<ABalloonKeepUpCharacter> OwnerCharacter;
	float ChargeRatio = 0.f;
	float MaxChargeTime = 1.f;
	float ChargeStartTime = 0.f;
};
