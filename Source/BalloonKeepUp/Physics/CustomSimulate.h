// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CustomSimulate.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCustomSimulate : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BALLOONKEEPUP_API ICustomSimulate
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void SimulatePhysics(float DeltaTime);

	virtual void SimulatePhysics_Implementation(float DeltaTime) {}
};
