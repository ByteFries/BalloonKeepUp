// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/CustomSimWorldSubsystem.h"

#include "CustomSimulate.h"

void UCustomSimWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	bIsActive = true;
	if (PhysicsTickInterval <= 0.f) PhysicsTickInterval = 1.f / 60.f;
}

void UCustomSimWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UCustomSimWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActive || GetWorld()->GetNetMode() == NM_Client) return;

	RemainingTime += DeltaTime;

	const float FixedDt = PhysicsTickInterval;
	const int32 MaxStepPerFrame = 6;

	int32 Steps = 0;

	while (RemainingTime >= FixedDt && Steps < MaxStepPerFrame)
	{
		
		for (TWeakObjectPtr<UObject> WeakObj : Objects)
		{
			if (UObject* Obj = WeakObj.Get())
			{
				if (Obj->GetClass()->ImplementsInterface(UCustomSimulate::StaticClass()))
					ICustomSimulate::Execute_SimulatePhysics(Obj, FixedDt);
			}
		}

		RemainingTime -= FixedDt;
		++Steps;
	}
}

TStatId UCustomSimWorldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCustomSimWorldSubsystem, STATGROUP_Tickables);
}

bool UCustomSimWorldSubsystem::Register(UObject* Object)
{
	if (GetWorld()->GetNetMode() == NM_Client || !Object) return false;
	
	if (!Object->GetClass()->ImplementsInterface(UCustomSimulate::StaticClass()))
	{
		return false;
	}

	Objects.AddUnique(Object);

	return true;
}

void UCustomSimWorldSubsystem::Unregister(UObject* Object)
{
	if ((GetWorld()->GetNetMode() == NM_Client) || !Objects.Contains(Object)) return;
	Objects.Remove(Object);
}
