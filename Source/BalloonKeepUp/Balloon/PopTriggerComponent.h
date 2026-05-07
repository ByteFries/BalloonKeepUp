// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PopTriggerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BALLOONKEEPUP_API UPopTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPopTriggerComponent();
};
