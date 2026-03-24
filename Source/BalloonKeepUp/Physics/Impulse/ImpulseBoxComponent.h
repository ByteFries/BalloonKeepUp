// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ImpulseTypes.h"
#include "Components/BoxComponent.h"
#include "Runtime/SlateCore/Private/Application/ActiveTimerHandle.h"
#include "ImpulseBoxComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BALLOONKEEPUP_API UImpulseBoxComponent : public UBoxComponent
{
	GENERATED_BODY()
public:
	UImpulseBoxComponent();
	
	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime,
		enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
		) override;
	
	UFUNCTION(BlueprintCallable)
	void ActivateVolume(float ActiveTime = 0.f);

	UFUNCTION(BlueprintCallable)
	void DeactivateVolume();

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);

	UFUNCTION()
	void HandleEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
protected:
	FImpulseContext BuildContext(AActor* OtherActor) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Impulse")
	FImpulseVolumeCommonData CommonData;

	FTimerHandle ActiveTimerHandle;
};
