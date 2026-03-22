// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ImpulseVolumeFunctionLibrary.generated.h"

struct FImpulseVolumeCommonData;
/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API UImpulseVolumeFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static void Activate(UPrimitiveComponent* Comp, FImpulseVolumeCommonData& Data);
	static void Deactivate(UPrimitiveComponent* Comp, FImpulseVolumeCommonData& Data);
	static void DrawDebug(UShapeComponent* Comp, float DebugThickness = 0, FColor Color = FColor::Green);
};
