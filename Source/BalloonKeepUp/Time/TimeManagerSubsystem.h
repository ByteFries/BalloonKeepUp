// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TimeManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API UTimeManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	bool Register(UObject* Object);
	void Unregister(UObject* Object);

	void SetPaused(bool Paused) {bPaused = Paused;}
private:
	float MakeCustomDelta(float DeltaTime) const;
	
	TArray<TWeakObjectPtr<UObject>> Objects;

	int32 MaxStepPerFrame = 6;

	float TickInterval = 1.f / 60.f;
	float RemainingTime = 0.f;
	float TimeScale = 1.f;
	float MaxDeltaSeconds;
	float HitchSkipThreshold = 0.3f;	
	
	bool bPaused = true;
};


