// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "State/StateBase.h"
#include "State_Dive.generated.h"

class IDiveStateOwner;

UCLASS()
class BALLOONKEEPUP_API UState_Dive : public UStateBase
{
	GENERATED_BODY()
public:
	virtual bool Init(UStateMachineComponent* InMachine, AActor* InOwner) override;
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Cancel() override;
private:
	TObjectPtr<IDiveStateOwner> DiveOwner;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dive", meta = (AllowPrivateAccess = "true"))
	float DivePower = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dive", meta = (AllowPrivateAccess = "true"))
	float DiveZ = 100.f;

};
