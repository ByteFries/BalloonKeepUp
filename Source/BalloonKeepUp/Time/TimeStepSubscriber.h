// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TimeStepSubscriber.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTimeStepSubscriber : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BALLOONKEEPUP_API ITimeStepSubscriber
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void OnFixedStep(float FixedDeltaTime);
};
