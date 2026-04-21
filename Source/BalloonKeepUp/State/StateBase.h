// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Public/GameplayInputTypes.h"
#include "UObject/NoExportTypes.h"
#include "StateBase.generated.h"

enum class ETriggerEvent : uint8;
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

	void HandleInput(const EInputAction Action, const ETriggerEvent Event);
	
	virtual void Commit() {}
	virtual void Cancel() {}
	
	
protected:
	virtual void HandlePressed(const EInputAction Action) {}
	virtual void HandleTriggered(const EInputAction Action) {}
	virtual void HandleReleased(const EInputAction Action) {}
	virtual void HandleCanceled(const EInputAction Action) {}
	UPROPERTY()
	TObjectPtr<UStateMachineComponent> Machine;

	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;
};
