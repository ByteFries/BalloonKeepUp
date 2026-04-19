// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(Blueprintable)
enum class EInputAction : uint8
{
	None,
	Jump,
	Dive,
	Receive,
	Spike
};