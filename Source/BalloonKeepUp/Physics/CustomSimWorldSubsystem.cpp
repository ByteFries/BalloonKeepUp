// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/CustomSimWorldSubsystem.h"
#include "CustomSimulate.h"
#include "Time/TimeManagerSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogCustomSim, Log, All);

void UCustomSimWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (GetWorld()->GetNetMode() == NM_Client)
	{
		return;
	}

	Collection.InitializeDependency(UTimeManagerSubsystem::StaticClass());
	
	bIsActive = true;
	bIsSimulating = false;
	
	UTimeManagerSubsystem* TimeManager = GetWorld()->GetSubsystem<UTimeManagerSubsystem>();

	if (!TimeManager)
	{
		ensureMsgf(false, TEXT("NoTimeManager"));
		return;
	}

	if (!TimeManager->Register(this))
	{
		UE_LOG(LogCustomSim, Warning, TEXT("Register failed: %s"), *GetName());
	}
}

void UCustomSimWorldSubsystem::Deinitialize()
{
	if (UTimeManagerSubsystem* TimeManager = GetWorld()->GetSubsystem<UTimeManagerSubsystem>())
	{
		TimeManager->Unregister(this);
	}
	Super::Deinitialize();
}

bool UCustomSimWorldSubsystem::Register(UObject* Object)
{
	if (GetWorld()->GetNetMode() == NM_Client || !Object) return false;
	
	if (!Object->GetClass()->ImplementsInterface(UCustomSimulate::StaticClass())) return false;

	if (Subscribers.Contains(Object) || PendingAdds.Contains(Object)) return false;
	
	if (bIsSimulating) PendingAdds.AddUnique(Object);
	else
	{
		Subscribers.AddUnique(Object);
		PendingRemoves.Remove(Object);
	}
	
	return true;
}

void UCustomSimWorldSubsystem::Unregister(UObject* Object)
{
	if ((GetWorld()->GetNetMode() == NM_Client) || !Subscribers.Contains(Object) || PendingRemoves.Contains(Object)) return;

	if (bIsSimulating) PendingRemoves.AddUnique(Object);
	else
	{
		ICustomSimulate::Execute_OnRemovedFromSimulation(Object);
		Subscribers.Remove(Object);
		PendingAdds.Remove(Object);
	}
}

void UCustomSimWorldSubsystem::OnFixedStep_Implementation(float FixedDeltaTime)
{
	if (!bIsActive) return;

	bIsSimulating = true;
	
	for (const TWeakObjectPtr<UObject>& WeakObj : Subscribers)
	{
		if (UObject* Obj = WeakObj.Get())
		{
			ICustomSimulate::Execute_SimulatePhysics(Obj, FixedDeltaTime);
		}
	}

	bIsSimulating = false;
	
	FlushPendingRemoves();
	FlushPendingAdds();
}

void UCustomSimWorldSubsystem::FlushPendingAdds()
{
	for (auto Obj : PendingAdds)
	{
		if (!Obj.IsValid()) continue;
		
		Subscribers.AddUnique(Obj);
	}

	PendingAdds.Empty();
}

void UCustomSimWorldSubsystem::FlushPendingRemoves()
{
	for (auto Obj : PendingRemoves)
	{
		Subscribers.Remove(Obj);

		if (!Obj.IsValid()) continue;
		
		ICustomSimulate::Execute_OnRemovedFromSimulation(Obj.Get());
	}

	PendingRemoves.Empty();

	Subscribers.RemoveAll([](const TWeakObjectPtr<UObject>& Ptr)
	{
		return !Ptr.IsValid();
	});
}
