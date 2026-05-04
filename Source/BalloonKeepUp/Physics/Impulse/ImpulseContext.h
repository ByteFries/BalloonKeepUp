// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ImpulseTypes.h"
#include "Fragment/ImpulseFragment.h"
#include "ImpulseContext.generated.h"
/**
 * 
 */

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
	
	UPROPERTY(EditAnywhere, Category = "Impulse")
	FVector DirectionBias = FVector::ZeroVector;
	// 특정 축에 가중치 넣어서 조절할 때
	UPROPERTY(EditAnywhere, Category = "Impulse")
	FVector AxisWeight = FVector(1.f, 1.f, 1.f); 

	UPROPERTY(EditAnywhere, Category = "Impulse")
	float BasePower = 0.f;

	UPROPERTY(EditAnywhere, Category = "Impulse")
	TMap<TSubclassOf<UImpulseFragment>, TObjectPtr<UImpulseFragment>> ImpulseFragments;
	
	template<typename T>
	const T* GetFragment() const
	{
		static_assert(TIsDerivedFrom<T, UImpulseFragment>::IsDerived, 
	"T must derive from UImpulseFragment");
		
		if (const TObjectPtr<UImpulseFragment>* Found = ImpulseFragments.Find(T::StaticClass()))
		{
			UImpulseFragment* Frag = Found->Get();
			if (!IsValid(Frag)) return nullptr;
			return Cast<T>(Frag);
		}

		return nullptr;
	}
};