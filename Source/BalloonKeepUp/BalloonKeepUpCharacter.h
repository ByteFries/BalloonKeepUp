// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Physics/Impulse/Fragment/ImpulseFragmentProvider.h"
#include "BalloonKeepUpCharacter.generated.h"

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
	Receive,
	Spike,
	Jump,
	Dive
};

UCLASS(abstract)
class ABalloonKeepUpCharacter : public ACharacter, public IImpulseFragmentProvider
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
	void OnChargeStarted(ECharacterState State);

	UFUNCTION(Server, Unreliable)
	void Server_RequestStartCharge(ECharacterState State);

	//UFUNCTION(BlueprintCallable, Category="Charge")
	//void OnChargeTriggered();
	
	UFUNCTION(BlueprintCallable, Category="Charge")
	void OnChargeCompleted();

	UFUNCTION(Server, Unreliable)
	void Server_CommitCharge();

	UFUNCTION(BlueprintCallable, Category="Charge")
	void OnChargeCanceled();

	UFUNCTION(Server, Unreliable)
	void Server_CancelCharge();
	
	UFUNCTION(BlueprintCallable)
	void OnDivePressed();

	UFUNCTION(Server, Unreliable)
	void Server_RequestDive();

	UFUNCTION(BlueprintCallable)
	void OnDiveEnd();

	UFUNCTION(Server, Unreliable)
	void Server_EndDive();
	
	virtual void ProvideFragments_Implementation(FImpulseContext& Context) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_ChangeState();

	void SetState(ECharacterState NewState);

	virtual void Landed(const FHitResult& Hit) override;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Input")
	void ExecuteSpike(float ActiveTime = 0.5f);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void ExecuteReceive(float ActiveTime = 0.5f);
	
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
	
	bool bIsCharging = false;
	
	float ChargeStartTime = 0.f;

	float MaxChargeTime = 1.f;

	float ChargeRatio = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dive", meta = (AllowPrivateAccess = "true"))
	float DivePower = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dive", meta = (AllowPrivateAccess = "true"))
	float DiveZ = 100.f;

	UPROPERTY(ReplicatedUsing=OnRep_ChangeState)
	ECharacterState ReplicatedState = ECharacterState::Idle;
	
	ECharacterState PredictState = ECharacterState::Idle;
};




