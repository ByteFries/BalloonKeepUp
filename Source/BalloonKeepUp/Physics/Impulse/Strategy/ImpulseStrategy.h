// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ImpulseTypes.h"
#include "UObject/NoExportTypes.h"
#include "ImpulseStrategy.generated.h"
struct FImpulseContext;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class BALLOONKEEPUP_API UImpulseStrategy : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FImpulseRequest Compute(const FImpulseContext& Context) const;
	
	static FVector ResolveDirection(const FImpulseContext& Context, const FVector& Direction);
};
