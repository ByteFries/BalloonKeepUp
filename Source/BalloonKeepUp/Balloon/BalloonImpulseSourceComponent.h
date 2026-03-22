// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BalloonImpulseSourceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BALLOONKEEPUP_API UBalloonImpulseSourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBalloonImpulseSourceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPrimitiveComponent> TargetPrimitive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StrengthScale = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinImpulse = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxImpulse = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UpBias = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabled = true;
};
