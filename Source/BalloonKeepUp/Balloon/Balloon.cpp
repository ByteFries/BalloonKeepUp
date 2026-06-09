// Fill out your copyright notice in the Description page of Project Settings.


#include "Balloon/Balloon.h"

#include "PopTriggerComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/BalloonRelayGameMode.h"
#include "GameFramework/BalloonRelayGameState.h"
#include "Net/UnrealNetwork.h"
#include "Physics/CustomSimWorldSubsystem.h"
#include "Physics/Impulse/ImpulseTypes.h"

ABalloon::ABalloon()
{
 	PrimaryActorTick.bCanEverTick = true;
	CollisionComp = CreateDefaultSubobject<USphereComponent>("Collision");
	RootComponent = CollisionComp;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetupAttachment(RootComponent);

	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bReplicates = true;
}

void ABalloon::BeginPlay()
{
	Super::BeginPlay();

	NoiseSeed = FMath::FRandRange(0.f, 10000.f);
	TargetLocation = GetActorLocation();
	TargetRotation = GetActorRotation();
	
	if (UCustomSimWorldSubsystem* Sim = GetWorld()->GetSubsystem<UCustomSimWorldSubsystem>())
	{
		Sim->Register(this);
	}

	OnActorBeginOverlap.AddDynamic(this, &ABalloon::HandleBeginOverlap);

	if (!HasAuthority()) return;

	if (ABalloonRelayGameState* GS = GetWorld()->GetGameState<ABalloonRelayGameState>())
	{
		GS->OnPlayEnableChanged.AddUObject(this, &ABalloon::HandlePlayEnableChanged); 
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
	const bool bShouldLog = false;
	
	if (bShouldLog)
	{
		UE_LOG(LogTemp, Log, TEXT("=== SimulatePhysics Start ==="));
		UE_LOG(LogTemp, Log, TEXT("Velocity Start: %s"), *Velocity.ToString());
		//bSkipClampOnce = true;
	}

	ApplyImpulse();
	
	FVector BuoyancyForce = GetBuoyancyForce();

	FVector DragForce = GetDragForce();
	
	ApplyWobble(DeltaTime);

	FVector Force = BuoyancyForce + DragForce;
	//////////// 오일러 적분 /////////////
	const FVector Acceleration = Force / Mass;
	Velocity += Acceleration * DeltaTime;
	if (bShouldLog)
	{
		UE_LOG(LogTemp, Log, TEXT("Total Force: %s"), *Force.ToString());
		UE_LOG(LogTemp, Log, TEXT("Acceleration: %s"), *Acceleration.ToString());
		UE_LOG(LogTemp, Log, TEXT("Velocity Before Move: %s"), *Velocity.ToString());
	}
	
	MoveWithSweepAndBounce(DeltaTime);

	//////////// 각속도 처리 //////////////
	const FQuat DeltaRot = FQuat(FRotator::MakeFromEuler(AngularVelocity * DeltaTime));
	AngularVelocity *= FMath::Pow(AngularDamping, DeltaTime);
	AddActorWorldRotation(DeltaRot);
	
	ReplicatedLocation = GetActorLocation();
	ReplicatedRotation = GetActorRotation();
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
		SetActorRotation(TargetRotation);
	}
	else
	{
		FVector NewLoc = FMath::VInterpTo(Current, TargetLocation, DeltaTime, LinearSmoothSpeed);
		AddActorWorldOffset(NewLoc - Current, false);

		FQuat NewQuat =
	FMath::QInterpTo(
		GetActorQuat(),
		TargetRotation.Quaternion(),
		DeltaTime,
		RotationSmoothSpeed);

		SetActorRotation(NewQuat);
	}
}

void ABalloon::ReceiveImpulseRequest_Implementation(const FImpulseRequest& Request)
{
	if (!IsActive || !bCanBeHit) return;
	
	//UE_LOG(LogTemp, Log, TEXT("[Balloon] ReceiveImpulseRequest"));
	//UE_LOG(LogTemp, Log, TEXT("  Power: %.2f"), Request.Power);
	//UE_LOG(LogTemp, Log, TEXT("  Direction: %s"), *Request.Direction.ToString());
	
	const FVector AppliedImpulse = Request.Power * Request.Direction;
	
	//UE_LOG(LogTemp, Log, TEXT("  AppliedImpulse: %s"), *AppliedImpulse.ToString());
	//UE_LOG(LogTemp, Log, TEXT("  PendingImpulse (Before): %s"), *PendingImpulse.ToString());
	PendingImpulse += AppliedImpulse;

	FVector Lever = Request.HitLocation - GetActorLocation();

	FVector Torque = FVector::CrossProduct(Lever, AppliedImpulse);

	AngularVelocity += Torque * AngularImpulseScale;

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABalloon::StartHitCooldown);
}

void ABalloon::HandlePlayEnableChanged(bool bEnable)
{
	SetFreeze(!bEnable);
}

void ABalloon::SetFreeze(bool bFreeze)
{
	if (!HasAuthority()) return;

	bFrozen = bFreeze;
	OnRep_Frozen();
}

void ABalloon::RequestPop()
{
	// fx 호출 및 destroy
	UE_LOG(LogTemp, Warning, TEXT("RequestPop"));
	if (auto CustomSim = GetWorld()->GetSubsystem<UCustomSimWorldSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("CustomSim"));
		CustomSim->Unregister(this);
	}
}

void ABalloon::ApplyImpulse()
{
	if (!PendingImpulse.IsNearlyZero())
	{
		Velocity += PendingImpulse / FMath::Max(Mass, KINDA_SMALL_NUMBER);
		//if (bShouldLog) UE_LOG(LogTemp, Log, TEXT("Velocity After Impulse: %s"), *Velocity.ToString());
		Velocity = Velocity.GetClampedToMaxSize(MaxImpulseSpeed);
		//if (bShouldLog) UE_LOG(LogTemp, Log, TEXT("Velocity After Clamp: %s"), *Velocity.ToString());
		PendingImpulse = FVector::ZeroVector;
	}
}

FVector ABalloon::GetBuoyancyForce()
{
	FVector Force = FVector::UpVector * BuoyancyStrength;	
	Force += FVector(0,0,Mass * GetWorld()->GetGravityZ());
	return Force;
}

FVector ABalloon::GetDragForce()
{
	FVector Force = FVector::ZeroVector;
	
	const float Speed = Velocity.Size();
	
	if (Speed > KINDA_SMALL_NUMBER)
	{
		const FVector LinearDrag = -K1 * Velocity;
		const FVector QuadDrag = -K2 * Velocity * Speed;

		//if (bShouldLog)
		//{
		//	UE_LOG(LogTemp, Log, TEXT("Speed Before Drag: %.2f"), Speed);
		//	UE_LOG(LogTemp, Log, TEXT("LinearDrag: %s"), *LinearDrag.ToString());
		//	UE_LOG(LogTemp, Log, TEXT("QuadDrag: %s"), *QuadDrag.ToString());
		//}
		
		Force += LinearDrag;
		Force += QuadDrag;
	}

	return Force;
}

void ABalloon::ApplyWobble(float DeltaTime)
{
	
	float T = GetWorld()->GetTimeSeconds();

	float Noise =
		FMath::PerlinNoise1D(T * NoiseFreq + NoiseSeed);

	float WobbleX =
		FMath::Sin(T * WobbleXFreq + Noise * 2.f);

	float WobbleY =
		FMath::Sin(T * WobbleYFreq + NoiseSeed + Noise * 2.f);

	Velocity += FVector(WobbleX, WobbleY, 0.f)
		* WobbleStrength
		* DeltaTime;
	
	float Roll =
	FMath::Sin(T * 1.1f + Noise * 2.f) * 15.f;

	float Pitch =
		FMath::Sin(T * 1.4f + NoiseSeed) * 10.f;

	float Bob =
		FMath::Sin(T * 1.2f + NoiseSeed) * 5.f;

	MeshComp->SetRelativeRotation(
		FRotator(Pitch, 0.f, Roll));

	MeshComp->SetRelativeLocation(
		FVector(0.f, 0.f, Bob));
}

void ABalloon::OnRemovedFromSimulation_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Balloon Pop"));
	Destroy();
}

void ABalloon::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!HasAuthority() || !bCanBeHit) return;

	//UE_LOG(LogTemp, Warning, TEXT("BeginOverlap"));
	if (ABalloonRelayGameMode* GM = GetWorld()->GetAuthGameMode<ABalloonRelayGameMode>())
	{
		GM->HandleBalloonOverlap(this, OtherActor);
	}
}

void ABalloon::HandleEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("EndOverlap"));
}

void ABalloon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABalloon, ReplicatedLocation);
	DOREPLIFETIME(ABalloon, ReplicatedRotation);
	DOREPLIFETIME(ABalloon, bFrozen);
}

void ABalloon::OnRep_ServerLocation()
{
	if (HasAuthority()) return;
	TargetLocation = ReplicatedLocation;
}

void ABalloon::OnRep_ServerRotation()
{
	if (HasAuthority()) return;
	TargetRotation = ReplicatedRotation;
}

void ABalloon::OnRep_Frozen()
{
	IsActive = !bFrozen;
}

///// 충돌 처리
void ABalloon::MoveWithSweepAndBounce(float DeltaTime)
{
	if (!CollisionComp) return;

	float RemainingTime = DeltaTime;
	const int32 MaxIterations = 3;
	constexpr float SurfaceFriction = 0.9f;
	constexpr float StopSpeed = 5.f;
	constexpr float PushOutEpsilon = 0.1f;
	const float BounceThreshold = 50.f;

	// delta time 만큼의 시뮬레이션 반복. 만약 이 간격 내에 충돌이 여러 번 발생하면 모두 처리해야하기 때문.
	for (int32 Iter = 0; Iter < MaxIterations && RemainingTime > KINDA_SMALL_NUMBER; ++Iter)
	{
		const FVector Delta = Velocity * RemainingTime;
		if (Delta.IsNearlyZero())
		{
			// 거의 움직이지 않으면 끝
			break;
		}

		FHitResult Hit;
		CollisionComp->MoveComponent(Delta, CollisionComp->GetComponentQuat(), true, &Hit);

		if (!Hit.bBlockingHit) break; // 충돌하지 않는다면 반복할 필요 없이 끝

		/*
		UE_LOG(LogTemp, Warning, TEXT("[Balloon] BlockingHit: Actor=%s Comp=%s Normal=%s Time=%.3f"),
		*GetNameSafe(Hit.GetActor()),
		*GetNameSafe(Hit.GetComponent()),
		*Hit.ImpactNormal.ToString(),
		Hit.Time);
		*/
		
		RemainingTime *= (1.f - Hit.Time); // 부딫혔다면 남은 시간 구하기.
		
		const FVector N = Hit.ImpactNormal.GetSafeNormal();
		const float VN = FVector::DotProduct(Velocity, N);

		if (VN < 0.f)
		{
			const FVector NormalVel = VN * N;
			FVector TangentVel = Velocity - NormalVel;

			TangentVel *= SurfaceFriction;

			if (-VN < BounceThreshold) // 약하게 충돌하면 흘러가듯
			{
				Velocity = TangentVel;

				if (Velocity.SizeSquared() < FMath::Square(StopSpeed))
				{
					Velocity = FVector::ZeroVector;
				}
			}
			else // 아니면 그대로 튕겨나도록
			{
				Velocity = TangentVel - NormalVel * Restitution;
			}
		}
		
		CollisionComp->AddWorldOffset(N * PushOutEpsilon, false); // 오브젝트 끼임 방지
	}
}

void ABalloon::StartHitCooldown()
{
	bCanBeHit = false;

	GetWorld()->GetTimerManager().SetTimer(CooldownHandle, this, &ABalloon::ResetHitCooldown, 0.3f, false);
}

void ABalloon::ResetHitCooldown()
{
	bCanBeHit = true;
}
