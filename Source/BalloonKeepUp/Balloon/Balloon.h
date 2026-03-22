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
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void ReceiveImpulseRequest_Implementation(const FImpulseRequest& Request) override;
private:
	void MoveWithSweepAndBounce(float DeltaTime);

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> CollisionComp;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMeshComponent> MeshComp;

	UPROPERTY(EditDefaultsOnly)
	float Mass = 1.f;
	
	UPROPERTY(EditDefaultsOnly)
	float BuoyancyStrength = 700.f; // 부력
	
	UPROPERTY(EditDefaultsOnly)
	float K1 = 0.5f; // 항력계수
	
	UPROPERTY(EditDefaultsOnly)
	float K2 = 0.1f; // Quad

	UPROPERTY(EditDefaultsOnly)
	float Restitution = 0.3f;
	
	//UPROPERTY(EditDefaultsOnly)
	//float Damping;

	UPROPERTY(EditDefaultsOnly)
	float TangentDamping = 0.6;

	FVector Velocity = FVector::ZeroVector;

	// 연속 노이즈 추가?
	
	bool IsActive = true;

	UPROPERTY(EditDefaultsOnly, Category="Baloon|Phsysics")
	float MaxImpulseSpeed = 1200.f;

	FVector PendingImpulse = FVector::ZeroVector;
};


