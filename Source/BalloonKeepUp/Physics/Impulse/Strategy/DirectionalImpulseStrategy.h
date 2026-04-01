// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Physics/Impulse/Strategy/ImpulseStrategy.h"
#include "DirectionalImpulseStrategy.generated.h"
/**
 * 
 */
UCLASS(BlueprintType, EditInlineNew)
class BALLOONKEEPUP_API UDirectionalImpulseStrategy : public UImpulseStrategy
{
	GENERATED_BODY()
public:
	
	virtual FImpulseRequest Compute_Implementation(const FImpulseContext& Context) const override;
};
