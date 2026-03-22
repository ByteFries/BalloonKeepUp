// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Impulse/ImpulseBoxComponent.h"

#include "ImpulseVolumeFunctionLibrary.h"

UImpulseBoxComponent::UImpulseBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetGenerateOverlapEvents(false);

	SetCollisionObjectType(ECC_WorldDynamic);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void UImpulseBoxComponent::BeginPlay()
{
	Super::BeginPlay();
	PrimaryComponentTick.bCanEverTick = true;
	
	OnComponentBeginOverlap.AddUniqueDynamic(this, &UImpulseBoxComponent::HandleBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UImpulseBoxComponent::HandleEndOverlap);
}

void UImpulseBoxComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!CommonData.bIsActive || !CommonData.bDebugDraw)
	{
		return;
	}

	DrawDebugBox(
		GetWorld(),
		GetComponentLocation(),
		GetScaledBoxExtent(),
		GetComponentQuat(),
		FColor::Cyan,
		false,
		0.f,
		0,
		1.f
	);
}

void UImpulseBoxComponent::ActivateVolume(float ActiveTime)
{
	UImpulseVolumeFunctionLibrary::Activate(this, CommonData);
	
	UE_LOG(LogTemp, Warning, TEXT("Before Enable: Registered=%d CanEverTick=%d TickEnabled=%d"),
		IsRegistered(),
		PrimaryComponentTick.bCanEverTick,
		IsComponentTickEnabled());

	SetComponentTickEnabled(true);

	UE_LOG(LogTemp, Warning, TEXT("After Enable: Registered=%d CanEverTick=%d TickEnabled=%d"),
		IsRegistered(),
		PrimaryComponentTick.bCanEverTick,
		IsComponentTickEnabled());
	
	
	if (ActiveTime > 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(ActiveTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			ActiveTimerHandle,
			this,
			&UImpulseBoxComponent::DeactivateVolume,
			ActiveTime,
			false
		);
	}
}

void UImpulseBoxComponent::DeactivateVolume()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ActiveTimerHandle);
	}
	
	UImpulseVolumeFunctionLibrary::Deactivate(this, CommonData);
	SetComponentTickEnabled(false);
}

void UImpulseBoxComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("BeginOverlap: %s"), OtherActor ? *OtherActor->GetName() : TEXT("None"));
}

void UImpulseBoxComponent::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Log, TEXT("EndOverlap: %s"), OtherActor ? *OtherActor->GetName() : TEXT("None"));
}
