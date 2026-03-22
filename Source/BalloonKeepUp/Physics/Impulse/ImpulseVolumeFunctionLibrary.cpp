// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Impulse/ImpulseVolumeFunctionLibrary.h"

#include "ImpulseTypes.h"

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

void UImpulseVolumeFunctionLibrary::DrawDebug(const UShapeComponent* Comp, float DebugThickness)
{

}
