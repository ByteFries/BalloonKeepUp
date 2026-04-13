// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Physics/Impulse/Strategy/DirectionalImpulseStrategy.h"
#include "ImpulseStrategy_DirectionCharge.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, EditInlineNew)
class BALLOONKEEPUP_API UImpulseStrategy_DirectionCharge : public UDirectionalImpulseStrategy
{
	GENERATED_BODY()
public:
	virtual FImpulseRequest Compute_Implementation(const FImpulseContext& Context) const override;
};
