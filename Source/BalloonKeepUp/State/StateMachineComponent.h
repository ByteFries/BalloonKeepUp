// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateMachineComponent.generated.h"


enum class EInputAction : uint8;
class UStateBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BALLOONKEEPUP_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateMachineComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

	bool RequestTransition(const TSubclassOf<UStateBase>& NewStateClass, FName Reason = NAME_None);

	void HandleInput(EInputAction Action, EInputEvent Event);

	UStateBase* GetCurrentState() const {return CurrentState;}

private:
	bool CanTransition(UStateBase* From, TSubclassOf<UStateBase> To);
	void ChangeState_Internal(TSubclassOf<UStateBase> NextStateClass);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UStateBase> DefaultStateClass;

	UPROPERTY()
	TObjectPtr<UStateBase> CurrentState;

	bool bIsTransitioning = false;
};
