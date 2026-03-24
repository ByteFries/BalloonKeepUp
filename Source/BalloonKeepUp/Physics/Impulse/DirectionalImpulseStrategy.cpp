// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Impulse/DirectionalImpulseStrategy.h"

FImpulseRequest UDirectionalImpulseStrategy::Compute_Implementation(const FImpulseContext& Context) const
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
	
	UE_LOG(LogTemp, Log, TEXT("Final Direction: %s"), *Direction.ToString());
	
	float Power = Context.BasePower;
	Power *= FMath::Max(Context.ChargeRatio, 1.f);
	
	Request.Direction = FinalDirection;
	Request.Power = Power;

	return Request;
}
