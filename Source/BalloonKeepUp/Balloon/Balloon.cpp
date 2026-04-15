// Fill out your copyright notice in the Description page of Project Settings.


#include "Balloon/Balloon.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Physics/CustomSimWorldSubsystem.h"
#include "Physics/Impulse/ImpulseTypes.h"

// Sets default values
ABalloon::ABalloon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComp = CreateDefaultSubobject<USphereComponent>("Collision");
	RootComponent = CollisionComp;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetupAttachment(RootComponent);

	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bReplicates = true;
}

// Called when the game starts or when spawned
void ABalloon::BeginPlay()
{
	Super::BeginPlay();

	TargetLocation = GetActorLocation();
	
	if (UCustomSimWorldSubsystem* Sim = GetWorld()->GetSubsystem<UCustomSimWorldSubsystem>())
	{
		Sim->Register(this);
	}
}

void ABalloon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UCustomSimWorldSubsystem* Sim = GetWorld()->GetSubsystem<UCustomSimWorldSubsystem>())
	{
		Sim->Unregister(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ABalloon::SimulatePhysics_Implementation(float DeltaTime)
{
	if (!HasAuthority() || !IsActive) return;
	const bool bShouldLog = !PendingImpulse.IsNearlyZero();
	
	if (bShouldLog)
	{
		UE_LOG(LogTemp, Log, TEXT("=== SimulatePhysics Start ==="));
		UE_LOG(LogTemp, Log, TEXT("Velocity Start: %s"), *Velocity.ToString());
		//bSkipClampOnce = true;
	}
	
	if (!PendingImpulse.IsNearlyZero())
	{
		Velocity += PendingImpulse / FMath::Max(Mass, KINDA_SMALL_NUMBER);
		if (bShouldLog) UE_LOG(LogTemp, Log, TEXT("Velocity After Impulse: %s"), *Velocity.ToString());
		Velocity = Velocity.GetClampedToMaxSize(MaxImpulseSpeed);
		if (bShouldLog) UE_LOG(LogTemp, Log, TEXT("Velocity After Clamp: %s"), *Velocity.ToString());
		PendingImpulse = FVector::ZeroVector;
	}
	
	FVector Force = FVector::ZeroVector;
	const float GravityZ = GetWorld()->GetGravityZ();
	
	Force += FVector::UpVector * BuoyancyStrength;
	Force += FVector(0,0,Mass * GravityZ);
	
	const float Speed = Velocity.Size();
	
	if (Speed > KINDA_SMALL_NUMBER)
	{
		const FVector LinearDrag = -K1 * Velocity;
		const FVector QuadDrag = -K2 * Velocity * Speed;

		if (bShouldLog)
		{
			UE_LOG(LogTemp, Log, TEXT("Speed Before Drag: %.2f"), Speed);
			UE_LOG(LogTemp, Log, TEXT("LinearDrag: %s"), *LinearDrag.ToString());
			UE_LOG(LogTemp, Log, TEXT("QuadDrag: %s"), *QuadDrag.ToString());
		}
		
		Force += LinearDrag;
		Force += QuadDrag;
	}
	
	const FVector Acceleration = Force / Mass;
	Velocity += Acceleration * DeltaTime;
	if (bShouldLog)
	{
		UE_LOG(LogTemp, Log, TEXT("Total Force: %s"), *Force.ToString());
		UE_LOG(LogTemp, Log, TEXT("Acceleration: %s"), *Acceleration.ToString());
		UE_LOG(LogTemp, Log, TEXT("Velocity Before Move: %s"), *Velocity.ToString());
	}
	
	MoveWithSweepAndBounce(DeltaTime);
	
	ReplicatedLocation = GetActorLocation();
}

// Called every frame
void ABalloon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority()) return;
	if (!IsActive) return;

	const FVector Current = GetActorLocation();

	float Dist = FVector::Dist(Current, TargetLocation);

	if (Dist > 200.f)
	{
		SetActorLocation(TargetLocation, false); // 순간이동
	}
	else
	{
		FVector NewLoc = FMath::VInterpTo(Current, TargetLocation, DeltaTime, SmoothSpeed);
		AddActorWorldOffset(NewLoc - Current, true);
	}
}

void ABalloon::ReceiveImpulseRequest_Implementation(const FImpulseRequest& Request)
{
	if (!IsActive) return;
	
	UE_LOG(LogTemp, Log, TEXT("[Balloon] ReceiveImpulseRequest"));
	UE_LOG(LogTemp, Log, TEXT("  Power: %.2f"), Request.Power);
	UE_LOG(LogTemp, Log, TEXT("  Direction: %s"), *Request.Direction.ToString());
	
	const FVector AppliedImpulse = Request.Power * Request.Direction;
	
	UE_LOG(LogTemp, Log, TEXT("  AppliedImpulse: %s"), *AppliedImpulse.ToString());
	UE_LOG(LogTemp, Log, TEXT("  PendingImpulse (Before): %s"), *PendingImpulse.ToString());
	PendingImpulse += Request.Power * Request.Direction;
}

void ABalloon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABalloon, ReplicatedLocation);
}

void ABalloon::OnRep_ServerLocation()
{
	if (HasAuthority()) return;
	//SetActorLocation(ReplicatedLocation, false);
	TargetLocation = ReplicatedLocation;
}

void ABalloon::MoveWithSweepAndBounce(float DeltaTime)
{
	if (!CollisionComp) return;

	float RemainingTime = DeltaTime;
	const int32 MaxIterations = 3;
	constexpr float SurfaceFriction = 0.9f;
	constexpr float StopSpeed = 5.f;
	constexpr float PushOutEpsilon = 0.1f;
	const float BounceThreshold = 15.f;
	
	for (int32 Iter = 0; Iter < MaxIterations && RemainingTime > KINDA_SMALL_NUMBER; ++Iter)
	{
		const FVector Delta = Velocity * RemainingTime;
		if (Delta.IsNearlyZero())
		{
			break;
		}

		FHitResult Hit;
		CollisionComp->MoveComponent(Delta, CollisionComp->GetComponentQuat(), true, &Hit);

		if (!Hit.bBlockingHit) break;

		UE_LOG(LogTemp, Warning, TEXT("[Balloon] BlockingHit: Actor=%s Comp=%s Normal=%s Time=%.3f"),
	*GetNameSafe(Hit.GetActor()),
	*GetNameSafe(Hit.GetComponent()),
	*Hit.ImpactNormal.ToString(),
	Hit.Time);
		
		RemainingTime *= (1.f - Hit.Time);
		
		const FVector N = Hit.ImpactNormal.GetSafeNormal();
		const float VN = FVector::DotProduct(Velocity, N);

		if (VN < 0.f)
		{
			const FVector NormalVel = VN * N;
			FVector TangentVel = Velocity - NormalVel;

			TangentVel *= SurfaceFriction;

			if (-VN < BounceThreshold)
			{
				Velocity = TangentVel;

				if (Velocity.SizeSquared() < FMath::Square(StopSpeed))
				{
					Velocity = FVector::ZeroVector;
				}
			}
			else
			{
				Velocity = TangentVel - NormalVel * Restitution;
			}
		}
		
		CollisionComp->AddWorldOffset(N * PushOutEpsilon, false);
	}
}

