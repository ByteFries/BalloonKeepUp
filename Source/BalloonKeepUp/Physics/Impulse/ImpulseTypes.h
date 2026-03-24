// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ImpulseTypes.generated.h"

class UImpulseStrategy;
/**
 * 
 */
UENUM()
enum class EImpulseDirectionSpace : uint8
{
	World,
	OwnerLocal,
	VolumeLocal
};

USTRUCT(BlueprintType)
struct FImpulseVolumeCommonData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Impulse")
	bool bIsActive = false;

	UPROPERTY(EditAnywhere, Category = "Impulse")
	float BasePower = 0.f;

	UPROPERTY(EditAnywhere, Category = "Impulse")
	FVector BaseDirection = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category = "Impulse")
	FVector DirectionBias = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Impulse")
	FVector AxisWeight = FVector(1.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, Category = "Impulse")
	EImpulseDirectionSpace DirectionSpace = EImpulseDirectionSpace::World;

	UPROPERTY(EditAnywhere, Instanced, Category = "Impulse")
	TObjectPtr<UImpulseStrategy> Strategy;

	bool bDebugDraw = true;
	// 추후에 식별용 GameplayTag 넣을지도
	// 차징 관련 데이터도 fragment로 뺄 생각
};

USTRUCT(BlueprintType)
struct FImpulseContext
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Impulse")
	TObjectPtr<AActor> InstigatorActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Impulse")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Impulse")
	FTransform VolumeTransform;

	UPROPERTY(EditAnywhere, Category = "Impulse")
	EImpulseDirectionSpace DirectionSpace;
	
	UPROPERTY(EditAnywhere, Category = "Impulse")
	FVector BaseDirection = FVector::ZeroVector; 
	// 
	UPROPERTY(EditAnywhere, Category = "Impulse")
	FVector DirectionBias = FVector::ZeroVector;
	// 특정 축에 가중치 넣어서 조절할 때
	UPROPERTY(EditAnywhere, Category = "Impulse")
	FVector AxisWeight = FVector(1.f, 1.f, 1.f); 

	UPROPERTY(EditAnywhere, Category = "Impulse")
	float BasePower = 0.f;

	UPROPERTY(EditAnywhere, Category = "Impulse")
	float ChargeRatio = 0.f;
};

USTRUCT(BlueprintType)
struct FImpulseRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Direction;
	
	UPROPERTY(BlueprintReadWrite)
	float Power;
};