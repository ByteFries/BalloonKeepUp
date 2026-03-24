// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Impulse/ImpulseBoxComponent.h"

#include "BalloonKeepUpCharacter.h"
#include "ImpulseReceiver.h"
#include "ImpulseStrategy.h"
#include "ImpulseVolumeFunctionLibrary.h"

UImpulseBoxComponent::UImpulseBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetGenerateOverlapEvents(false);
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

	UImpulseVolumeFunctionLibrary::DrawDebug(this, 1.f);
}

void UImpulseBoxComponent::ActivateVolume(float ActiveTime)
{
	UImpulseVolumeFunctionLibrary::Activate(this, CommonData);
	
	SetComponentTickEnabled(true);
	
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

	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	if (!OtherActor->GetClass()->ImplementsInterface(UImpulseReceiver::StaticClass()))
	{
		return;
	}
	
	FImpulseContext Context = BuildContext(OtherActor);
	FImpulseRequest Request = CommonData.Strategy->Compute(Context);
	IImpulseReceiver::Execute_ReceiveImpulseRequest(OtherActor, Request);
}

void UImpulseBoxComponent::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Log, TEXT("EndOverlap: %s"), OtherActor ? *OtherActor->GetName() : TEXT("None"));
}

FImpulseContext UImpulseBoxComponent::BuildContext(AActor* OtherActor) const
{
	AActor* Owner = GetOwner();
	
	FImpulseContext Context;
	Context.InstigatorActor = Owner;
	Context.TargetActor = OtherActor;
	Context.BasePower = CommonData.BasePower;
	Context.DirectionBias = CommonData.DirectionBias;
	Context.BaseDirection = CommonData.BaseDirection;
	Context.AxisWeight = CommonData.AxisWeight;
	Context.VolumeTransform = GetComponentTransform();
	Context.DirectionSpace = CommonData.DirectionSpace;

	if (ABalloonKeepUpCharacter* Char = Cast<ABalloonKeepUpCharacter>(Owner))
	{
		Context.ChargeRatio = Char->GetChargeRatio();
	}

	return Context;
}
