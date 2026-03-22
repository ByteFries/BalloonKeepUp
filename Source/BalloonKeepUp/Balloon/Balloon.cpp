// Fill out your copyright notice in the Description page of Project Settings.


#include "Balloon/Balloon.h"

#include "BalloonImpulseSourceComponent.h"
#include "Components/SphereComponent.h"
#include "Math/UnitConversion.h"
#include "Physics/CustomSimWorldSubsystem.h"

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
}

// Called when the game starts or when spawned
void ABalloon::BeginPlay()
{
	Super::BeginPlay();

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

UBalloonImpulseSourceComponent* ABalloon::FindImpulseSource(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	if (!OtherActor || !OtherComp) return nullptr;

	TArray<UBalloonImpulseSourceComponent*> Sources;
	OtherActor->GetComponents<UBalloonImpulseSourceComponent>(Sources);

	for (UBalloonImpulseSourceComponent* Source : Sources)
	{
		if (!Source || !Source->bEnabled) continue;

		if (Source->TargetPrimitive == OtherComp)
		{
			return Source;
		}
	}

	return nullptr;
}

void ABalloon::OnBalloonBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !IsActive || !OtherActor || !OtherComp || OtherActor == this)
	{
		return;
	}

	UBalloonImpulseSourceComponent* Source = FindImpulseSource(OtherActor, OtherComp);
	if (!Source) return;

	const FVector Impulse = ComputeImpulseFromSource(*Source, OtherActor);
	
	if (!Impulse.IsNearlyZero())
	{
		AddPendingImpulse(Impulse);
	}
}

FVector ABalloon::ComputeImpulseFromSource(const UBalloonImpulseSourceComponent& Source,
	const AActor* SourceActor) const
{
	if (!SourceActor) return FVector::ZeroVector;

	FVector Dir = GetActorLocation() - SourceActor->GetActorLocation();
	Dir.Z += Source.UpBias;
	Dir = Dir.GetSafeNormal();

	if (Dir.IsNearlyZero()) return FVector::ZeroVector;

	const FVector SourceVelocity = SourceActor->GetVelocity();
	const float ApproachSpeed = FVector::DotProduct(SourceVelocity, SourceVelocity);

	float Strength = 0.f;

	if (ApproachSpeed > 0.f)
	{
		Strength = ApproachSpeed * Source.StrengthScale;
	}
	else
	{
		Strength = Source.MinImpulse;
	}

	Strength = FMath::Clamp(Strength, Source.MinImpulse, Source.MaxImpulse);

	return Dir * Strength;
}


void ABalloon::SimulatePhysics_Implementation(float DeltaTime)
{
	if (!HasAuthority() || !IsActive) return;

	if (!PendingImpulse.IsNearlyZero())
	{
		Velocity += PendingImpulse / FMath::Max(Mass, KINDA_SMALL_NUMBER);
		Velocity = Velocity.GetClampedToMaxSize(MaxImpulseSpeed);
		PendingImpulse = FVector::ZeroVector;
	}
	
	FVector Force = FVector::ZeroVector;
	const float GravityZ = GetWorld()->GetGravityZ();
	
	Force += FVector::UpVector * BuoyancyStrength;
	Force += FVector(0,0,Mass * GravityZ);
	
	const float Speed = Velocity.Size();

	if (Speed > KINDA_SMALL_NUMBER)
	{
		Force += -K1 * Velocity;
		Force += -K2 * Velocity * Speed;
	}

	const FVector Acceleration = Force / Mass;
	Velocity += Acceleration * DeltaTime;
	
	MoveWithSweepAndBounce(DeltaTime);
}

// Called every frame
void ABalloon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABalloon::AddPendingImpulse(const FVector& Impulse)
{
	if (!IsActive) return;

	PendingImpulse += Impulse;
}

void ABalloon::MoveWithSweepAndBounce(float DeltaTime)
{
	if (!CollisionComp) return;

	float RemainingTime = DeltaTime;
	const int32 MaxIterations = 3;
	constexpr float SurfaceFriction = 0.6f;
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
