// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Physics/CustomSimulate.h"
#include "Physics/Impulse/ImpulseReceiver.h"
#include "Balloon.generated.h"

class UBalloonImpulseSourceComponent;
class USphereComponent;

UCLASS()
class BALLOONKEEPUP_API ABalloon : public AActor, public ICustomSimulate, public IImpulseReceiver
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABalloon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SimulatePhysics_Implementation(float DeltaTime) override;
	// 히트 함수
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void SetFreeze(bool bFreeze);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void ReceiveImpulseRequest_Implementation(const FImpulseRequest& Request) override;
	
	void RequestPop();
private:
	void ApplyImpulse();

	FVector GetBuoyancyForce();
	
	FVector GetDragForce();

	void ApplyWobble(float DeltaTime);
	
	virtual void OnRemovedFromSimulation_Implementation() override;
	
	void HandlePlayEnableChanged(bool bEnable);
	
	UFUNCTION()
	void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void HandleEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_ServerLocation();
	
	UFUNCTION()
	void OnRep_ServerRotation();

	UFUNCTION()
	void OnRep_Frozen();
	
	void MoveWithSweepAndBounce(float DeltaTime);

	void StartHitCooldown();
	void ResetHitCooldown();

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> CollisionComp;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMeshComponent> MeshComp;

	bool IsActive = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_Frozen)
	bool bFrozen = true;
	
	FVector PendingImpulse = FVector::ZeroVector;

	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(ReplicatedUsing=OnRep_ServerLocation)
	FVector ReplicatedLocation;
	
	FVector TargetLocation;

	UPROPERTY(ReplicatedUsing=OnRep_ServerRotation)
	FRotator ReplicatedRotation;

	FRotator TargetRotation;
	
	UPROPERTY()
	FVector AngularVelocity;

	UPROPERTY(EditDefaultsOnly, Category="Balloon|Physics")
	float Mass = 1.f;

	//임펄스 최대 허용량
	UPROPERTY(EditDefaultsOnly, Category="Balloon|Physics")
	float MaxImpulseSpeed = 2400.f;

	//부력 계수
	UPROPERTY(EditDefaultsOnly, Category="Balloon|Physics")
	float BuoyancyStrength = 700.f;

	//선형 항력
	UPROPERTY(EditDefaultsOnly, Category="Balloon|Physics")
	float K1 = 0.5f;
	
	//
	UPROPERTY(EditDefaultsOnly, Category="Balloon|Physics")
	float K2 = 0.1f; // Quad

	UPROPERTY(EditDefaultsOnly, Category="Balloon|Physics")
	float Restitution = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category="Balloon|Physics")
	float TangentDamping = 0.6;

	UPROPERTY(EditAnywhere, Category="Balloon|Physics")
	float AngularImpulseScale = 0.01f;

	UPROPERTY(EditAnywhere, Category="Balloon|Physics")
	float AngularDamping = 0.01f;

	UPROPERTY(EditAnywhere, Category="Balloon|Physics")
	float MaxAngularSpeed;

	UPROPERTY(EditAnywhere, Category="Balloon|Wobble")
	float NoiseSeed;

	UPROPERTY(EditAnywhere, Category="Balloon|Wobble")
	float WobbleStrength = 25;

	UPROPERTY(EditAnywhere, Category="Balloon|Wobble")
	float NoiseFreq = 0.08f;

	UPROPERTY(EditAnywhere, Category="Balloon|Wobble")
	float WobbleXFreq = 0.1f;
	
	UPROPERTY(EditAnywhere, Category="Balloon|Wobble")
	float WobbleYFreq = 0.3f;
	
	UPROPERTY(EditAnywhere, Category="Balloon|Net Smoothing")
	float LinearSmoothSpeed = 24.f;

	UPROPERTY(EditAnywhere, Category="Balloon|Net Smoothing")
	float RotationSmoothSpeed = 24.f;
	
	FTimerHandle CooldownHandle;
	
	bool bCanBeHit = true;
};



