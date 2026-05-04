// Fill out your copyright notice in the Description page of Project Settings.


#include "Time/TimeManagerSubsystem.h"

#include "TimeStepSubscriber.h"


void UTimeManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bPaused = false;
	MaxDeltaSeconds = TickInterval * MaxStepPerFrame;
}

void UTimeManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UTimeManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPaused || GetWorld()->GetNetMode() == NM_Client) return;
	
	Objects.RemoveAll([](const TWeakObjectPtr<UObject>& Ptr)
	{
		return !Ptr.IsValid();
	});
	
	RemainingTime += MakeCustomDelta(DeltaTime);
	RemainingTime = FMath::Min(RemainingTime, TickInterval * MaxStepPerFrame);
	
	int32 Steps = 0;

	while (RemainingTime >= TickInterval && Steps < MaxStepPerFrame)
	{
		for (TWeakObjectPtr<UObject> WeakObj : Objects)
		{
			if (UObject* Obj = WeakObj.Get())
			{
				ITimeStepSubscriber::Execute_OnFixedStep(Obj, TickInterval);
			}
		}

		RemainingTime -= TickInterval;
		++Steps;
	}
	
}

TStatId UTimeManagerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTimeManagerSubsystem, STATGROUP_Tickables);
}

bool UTimeManagerSubsystem::Register(UObject* Object)
{
	if (GetWorld()->GetNetMode() == NM_Client || !Object) return false;

	if (!Object->GetClass()->ImplementsInterface(UTimeStepSubscriber::StaticClass()))
	{
		return false;
	}

	Objects.AddUnique(Object);
	
	return true;
}

void UTimeManagerSubsystem::Unregister(UObject* Object)
{
	if (GetWorld()->GetNetMode() == NM_Client || !Object || !Objects.Contains(Object)) return;
	Objects.Remove(Object);
	
}


float UTimeManagerSubsystem::MakeCustomDelta(float DeltaTime) const
{
	if (bPaused) return 0.f;

	if (DeltaTime > HitchSkipThreshold) return 0.f;

	float Delta = FMath::Clamp(DeltaTime, 0.f, MaxDeltaSeconds);
	Delta *= TimeScale;

	return Delta;
}
