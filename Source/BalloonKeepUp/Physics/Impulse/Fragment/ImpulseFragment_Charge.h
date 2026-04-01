// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Physics/Impulse/Fragment/ImpulseFragment.h"
#include "ImpulseFragment_Charge.generated.h"

/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API UImpulseFragment_Charge : public UImpulseFragment
{
	GENERATED_BODY()
public:
	float ChargeRatio;
};
