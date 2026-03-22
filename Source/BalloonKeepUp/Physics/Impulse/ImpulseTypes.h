// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ImpulseTypes.generated.h"

/**
 * 
 */
// context, request 추가할 예정
USTRUCT(BlueprintType)
struct FImpulseVolumeCommonData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Impulse")
	bool bIsActive = false;

	UPROPERTY(EditAnywhere, Category = "Impulse")
	float BaseStrength = 0.f;

	UPROPERTY(EditAnywhere, Category = "Impulse")
	FVector DirectionBias = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Impulse")
	FVector AxisWeight = FVector(1.f, 1.f, 1.f);

	bool bDebugDraw = true;
	// 추후에 식별용 GameplayTag 넣을지도
	// 차징 관련 데이터도 fragment로 뺄 생각
};

USTRUCT(BlueprintType)
struct FImpulseContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> InstigatorActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FVector SourceLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	float ChargeRatio = 0.f;
};

USTRUCT(BlueprintType)
struct FImpulseRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Impulse")
	FVector Impulse;
};