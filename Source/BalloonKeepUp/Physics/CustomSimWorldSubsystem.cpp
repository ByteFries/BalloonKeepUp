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
	Super::Deinitialize();

	UTimeManagerSubsystem* TimeManager = GetWorld()->GetSubsystem<UTimeManagerSubsystem>();

	if (!TimeManager)
	{
		return;
	}

	TimeManager->Unregister(this);
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

void UCustomSimWorldSubsystem::OnFixedStep_Implementation(float FixedDeltaTime)
{
	if (!bIsActive) return;
	
	Objects.RemoveAll([](const TWeakObjectPtr<UObject>& Ptr)
	{
		return !Ptr.IsValid();
	});
	
	for (const TWeakObjectPtr<UObject>& WeakObj : Objects)
	{
		if (UObject* Obj = WeakObj.Get())
		{
			ICustomSimulate::Execute_SimulatePhysics(Obj, FixedDeltaTime);
		}
	}
}
