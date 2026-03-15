// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CustomSimWorldSubsystem.generated.h"

class ICustomSimulate;
/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API UCustomSimWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	
	bool Register(UObject* Object);
	void Unregister(UObject* Object);
private:
	TArray<TWeakObjectPtr<UObject>> Objects;

	UPROPERTY(EditDefaultsOnly)
	float PhysicsTickInterval = 1/60.f;

	float RemainingTime = 0.f; // 누적량

	bool bIsActive = false;
};
