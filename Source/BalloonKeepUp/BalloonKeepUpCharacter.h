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

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	bool bIsCharging = false;
	
	float ChargeStartTime = 0.f;

	float MaxChargeTime = 1.f;

	float ChargeRatio = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dive", meta = (AllowPrivateAccess = "true"))
	float DivePower = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dive", meta = (AllowPrivateAccess = "true"))
	float DiveZ = 100.f;
	
	
	ECharacterState CurrentState = ECharacterState::Idle;
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SpikeAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ReceiveAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* DiveAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Impulse", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UImpulseBoxComponent> SpikeBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Impulse", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UImpulseBoxComponent> NewReceiveBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Impulse", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UImpulseBoxComponent> NewDiveBox;

public:

	/** Constructor */
	ABalloonKeepUpCharacter();
protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UFUNCTION(BlueprintCallable, Category="Input")
	void ExecuteSpike(float ActiveTime = 0.5f);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void ExecuteReceive(float ActiveTime = 0.5f);

	UFUNCTION(BlueprintCallable, Category="Charge")
	void OnChargeStarted(ECharacterState State);

	UFUNCTION(BlueprintCallable, Category="Charge")
	void OnChargeTriggered();
	
	UFUNCTION(BlueprintCallable, Category="Charge")
	void OnChargeCompleted();

	UFUNCTION(BlueprintCallable, Category="Charge")
	void OnChargeCanceled();

	UFUNCTION(BlueprintCallable)
	void OnDivePressed();

	UFUNCTION(BlueprintCallable)
	void OnDiveEnd();

	virtual void ProvideFragments_Implementation(FImpulseContext& Context) override;
public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};



