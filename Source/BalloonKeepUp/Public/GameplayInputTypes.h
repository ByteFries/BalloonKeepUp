// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayInputTypes.generated.h"

UENUM(BlueprintType)
enum class EInputAction : uint8
{
	None = 0,
	Jump,
	Dive,
	Receive,
	Spike
};