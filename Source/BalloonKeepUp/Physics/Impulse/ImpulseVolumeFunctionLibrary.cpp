// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Impulse/ImpulseVolumeFunctionLibrary.h"

#include "ImpulseTypes.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"

void UImpulseVolumeFunctionLibrary::Activate(UPrimitiveComponent* Comp, FImpulseVolumeCommonData& Data)
{
	Data.bIsActive = true;
	Comp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Comp->SetGenerateOverlapEvents(true);
}

void UImpulseVolumeFunctionLibrary::Deactivate(UPrimitiveComponent* Comp, FImpulseVolumeCommonData& Data)
{
	Data.bIsActive = false;
	Comp->SetGenerateOverlapEvents(false);
	Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UImpulseVolumeFunctionLibrary::DrawDebug(UShapeComponent* Comp, float DebugThickness, FColor Color)
{
	if (UBoxComponent* Box = Cast<UBoxComponent>(Comp))
	{
		DrawDebugBox(
			Box->GetWorld(),
		Box->GetComponentLocation(),
			Box->GetScaledBoxExtent(),
			Box->GetComponentQuat(),
			Color,
		false,
		0.f,
		0,
			DebugThickness
		);
	}
	else if (USphereComponent* Sphere = Cast<USphereComponent>(Comp))
	{
		//DrawDebugSphere(
		//	Sphere->GetWorld(),
		//	Sphere->GetComponentLocation(),
		//	Sphere->GetScaledSphereRadius(),
		//	Sphere->Get);
	}
	else if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Comp))
	{
		
	}
	
}
