// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Public/GameplyaInputTypes.h"
#include "UObject/NoExportTypes.h"
#include "StateBase.generated.h"

class UStateMachineComponent;
class ABalloonKeepUpCharacter;
/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API UStateBase : public UObject
{
	GENERATED_BODY()
public:
	virtual bool Init(UStateMachineComponent* InMachine, AActor* InOwner)
	{
		if (!InMachine || !InOwner) return false;
		
		Machine = InMachine;
		OwnerActor = InOwner;
		return true;
	}
	
	virtual void Enter() {}
	virtual void Tick(float DeltaTime) {}
	virtual void Exit() {}

	virtual void HandleInput(EInputAction Action, EInputEvent Event) {}
	
	virtual void Commit() {}
	virtual void Cancel() {}
	
	
protected:
	UPROPERTY()
	TObjectPtr<UStateMachineComponent> Machine;

	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;
};
