// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Impulse/Strategy/ImpulseStrategy_DirectionCharge.h"

#include "Physics/Impulse/ImpulseContext.h"
#include "Physics/Impulse/Fragment/ImpulseFragment_Charge.h"

FImpulseRequest UImpulseStrategy_DirectionCharge::Compute_Implementation(const FImpulseContext& Context) const
{
	FImpulseRequest Request;
	FVector Direction = Context.BaseDirection;
	Direction += Context.DirectionBias;

	Direction.X *= Context.AxisWeight.X;
	Direction.Y *= Context.AxisWeight.Y;
	Direction.Z *= Context.AxisWeight.Z;

	Direction = Direction.GetSafeNormal();
	
	UE_LOG(LogTemp, Log, TEXT("Direction: %s"), *Direction.ToString());
	
	
	FVector FinalDirection = ResolveDirection(Context, Direction);
	
	UE_LOG(LogTemp, Log, TEXT("Final Direction: %s"), *FinalDirection.ToString());
	
	float Power = Context.BasePower;
	
	Request.Direction = FinalDirection;
	Request.Power = Power;
	
	if (const UImpulseFragment_Charge* Charge = Context.GetFragment<UImpulseFragment_Charge>())
	{
		Request.Power *= Charge->ChargeRatio;
		UE_LOG(LogTemp, Log, TEXT("BasePower=%f, Ratio=%f, FinalPower=%f"),
		Context.BasePower,
		Charge->ChargeRatio,
		Request.Power);
	}
	return Request;
}
