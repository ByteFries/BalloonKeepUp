// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Impulse/ImpulseBoxComponent.h"

#include "BalloonKeepUpCharacter.h"
#include "ImpulseReceiver.h"
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
	
	// 인터페이스 검사, 쿨타임 확인
	FImpulseContext Context = BuildContext(OtherActor);
	//Strategy 계산
	// otherActor에 request 보내기
	FImpulseRequest Request;
	IImpulseReceiver::Execute_ReceiveImpulseRequest(OtherActor, Request);
}

void UImpulseBoxComponent::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Log, TEXT("EndOverlap: %s"), OtherActor ? *OtherActor->GetName() : TEXT("None"));
}

FImpulseContext UImpulseBoxComponent::BuildContext(AActor* OtherActor)
{
	FImpulseContext Context;
	Context.InstigatorActor = GetOwner();
	Context.SourceLocation = GetComponentLocation();
	Context.TargetActor = OtherActor;
	Context.TargetLocation = OtherActor->GetActorLocation();

	if (ABalloonKeepUpCharacter* Char = Cast<ABalloonKeepUpCharacter>(GetOwner()))
	{
		Context.ChargeRatio = Char->GetChargeRatio();
	}

	return Context;
}
