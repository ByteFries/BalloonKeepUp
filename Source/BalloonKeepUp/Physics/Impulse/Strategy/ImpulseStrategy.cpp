// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Impulse/Strategy/ImpulseStrategy.h"

#include "Physics/Impulse/ImpulseContext.h"

FImpulseRequest UImpulseStrategy::Compute_Implementation(const FImpulseContext& Context) const
{
	return FImpulseRequest{};
}

FVector UImpulseStrategy::ResolveDirection(const FImpulseContext& Context, const FVector& Direction)
{
	switch (Context.DirectionSpace)
	{
	case EImpulseDirectionSpace::World:
		return Direction.GetSafeNormal();

	case EImpulseDirectionSpace::OwnerLocal:
		return Context.InstigatorActor
			? Context.InstigatorActor->GetActorTransform()
				.TransformVectorNoScale(Direction)
				.GetSafeNormal()
			: Direction.GetSafeNormal();

	case EImpulseDirectionSpace::VolumeLocal:
		FVector FinalDirection = Context.VolumeTransform.TransformVectorNoScale(Direction).GetSafeNormal();
		
		UE_LOG(LogTemp, Log, TEXT("Transform: %s"), *Context.VolumeTransform.ToString());
		UE_LOG(LogTemp, Log, TEXT("Volume Direction: %s"), *FinalDirection.ToString());
		return FinalDirection;
	}

	return Direction.GetSafeNormal();
}
