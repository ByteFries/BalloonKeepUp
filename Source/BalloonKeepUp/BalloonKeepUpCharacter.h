// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedPlayerInput.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Physics/Impulse/Fragment/ImpulseFragmentProvider.h"
#include "State/DiveStateOwner.h"
#include "BalloonKeepUpCharacter.generated.h"

enum class EImpulseBoxType : uint8;
class UState_Receive;
enum class EInputAction : uint8;
class UStateMachineComponent;
class UStateBase;
class UImpulseFragment_Charge;
class UImpulseBoxComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UENUM(Blueprintable)
enum class ECharacterState : uint8
{
	Idle = 0,
	Jump,
	Dive,
	Charge
};

UCLASS(abstract)
class ABalloonKeepUpCharacter : public ACharacter, public IImpulseFragmentProvider, public IDiveStateOwner
{
	GENERATED_BODY()
public:
	ABalloonKeepUpCharacter();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UFUNCTION(BlueprintCallable, Category="Charge")
	void ChargeStart(const EInputAction Action);
	
	UFUNCTION(BlueprintCallable, Category="Charge")
	void ChargeComplete(const EInputAction Action);

	UFUNCTION(BlueprintCallable, Category="Charge")
	void ChargeCancel(const EInputAction Action);

	void DoSpike();

	void DoReceive();
	
	UFUNCTION(BlueprintCallable)
	void DiveStart();

	virtual void DoDive(FVector DiveVel) override;
	
	virtual void CancelDive() override;
	
	virtual void ProvideFragments_Implementation(FImpulseContext& Context) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	UFUNCTION(BlueprintCallable, Category="Request")
	void RequestChargeAction(const EInputAction Action, const float ChargeRatio);

	void EnableImpulseBox(const EImpulseBoxType Type, const float ActiveTime = 0.5f);
	void DeactivateImpulseBox(const EImpulseBoxType Type);
private:
	UFUNCTION(Server, Unreliable)
	void RequestHandleInput(const EInputAction Action, const ETriggerEvent TriggerEvent);
	
	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Diving();
	
	UFUNCTION()
	void OnRep_Spiking();
	
	UFUNCTION()
	void OnRep_Receiving();

	void OnSpikeMontageStart();
	void OnSpikeMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void OnReceiveMontageStart();
	void OnReceiveMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void ApplyDiveCapsule(bool IsDiving);
	
	virtual void Landed(const FHitResult& Hit) override;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateMachineComponent> StateMachine;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Impulse", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UImpulseBoxComponent> SpikeBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Impulse", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UImpulseBoxComponent> NewReceiveBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Impulse", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UImpulseBoxComponent> NewDiveBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SpikeAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ReceiveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* DiveAction;
	
	UPROPERTY(ReplicatedUsing=OnRep_Diving)
	bool bIsDiving = false;
	
	UPROPERTY(ReplicatedUsing=OnRep_Spiking)
	bool bIsSpiking = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Anim", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> SpikeMontage;
	
	UPROPERTY(ReplicatedUsing=OnRep_Receiving)
	bool bIsReceiving = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Anim", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ReceiveMontage;
	
	float ChargeRatio;
};




