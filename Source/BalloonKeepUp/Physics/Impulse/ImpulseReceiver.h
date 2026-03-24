// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ImpulseTypes.h"
#include "ImpulseReceiver.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UImpulseReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BALLOONKEEPUP_API IImpulseReceiver
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void ReceiveImpulseRequest(const FImpulseRequest& Request);
};
