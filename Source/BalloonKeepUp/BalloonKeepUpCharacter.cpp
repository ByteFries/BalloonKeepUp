// Copyright Epic Games, Inc. All Rights Reserved.

#include "BalloonKeepUpCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "BalloonKeepUp.h"
#include "Net/UnrealNetwork.h"
#include "Physics/Impulse/ImpulseBoxComponent.h"
#include "Physics/Impulse/Fragment/ImpulseFragment_Charge.h"

ABalloonKeepUpCharacter::ABalloonKeepUpCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	SpikeBox = CreateDefaultSubobject<UImpulseBoxComponent>(TEXT("SpikeBox"));
	SpikeBox->SetupAttachment(RootComponent);

	NewReceiveBox = CreateDefaultSubobject<UImpulseBoxComponent>(TEXT("NewReceiveBox"));
	NewReceiveBox->SetupAttachment(RootComponent);

	NewDiveBox = CreateDefaultSubobject<UImpulseBoxComponent>(TEXT("NewDiveBox"));
	NewDiveBox->SetupAttachment(RootComponent);

	bReplicates = true;
}


void ABalloonKeepUpCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Look);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Look);
		
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Started, this, &ABalloonKeepUpCharacter::OnChargeStarted, ECharacterState::Spike);
		//EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::OnChargeTriggered);
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Completed, this, &ABalloonKeepUpCharacter::OnChargeCompleted);
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Canceled, this, &ABalloonKeepUpCharacter::OnChargeCanceled);

		EnhancedInputComponent->BindAction(ReceiveAction, ETriggerEvent::Started, this, &ABalloonKeepUpCharacter::OnChargeStarted, ECharacterState::Receive);
		//EnhancedInputComponent->BindAction(ReceiveAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::OnChargeTriggered);
		EnhancedInputComponent->BindAction(ReceiveAction, ETriggerEvent::Completed, this, &ABalloonKeepUpCharacter::OnChargeCompleted);
		EnhancedInputComponent->BindAction(ReceiveAction, ETriggerEvent::Canceled, this, &ABalloonKeepUpCharacter::OnChargeCanceled);

		EnhancedInputComponent->BindAction(DiveAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::OnDivePressed);
	}
	else
	{
		UE_LOG(LogBalloonKeepUp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABalloonKeepUpCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABalloonKeepUpCharacter, ReplicatedState);
}

void ABalloonKeepUpCharacter::OnRep_ChangeState()
{
	PredictState = ReplicatedState;
	SetState(ReplicatedState);
}

void ABalloonKeepUpCharacter::SetState(ECharacterState NewState)
{
	if (NewState != ECharacterState::Dive)
		GetCapsuleComponent()->SetCapsuleHalfHeight(90, true);
	else
		GetCapsuleComponent()->SetCapsuleHalfHeight(40, false);
}

void ABalloonKeepUpCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (ReplicatedState == ECharacterState::Dive)
	{
		OnDiveEnd();
	}
}

void ABalloonKeepUpCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ABalloonKeepUpCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ABalloonKeepUpCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ABalloonKeepUpCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ABalloonKeepUpCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ABalloonKeepUpCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ABalloonKeepUpCharacter::ExecuteSpike(float ActiveTime)
{
	SpikeBox->ActivateVolume(ActiveTime);
}


void ABalloonKeepUpCharacter::ExecuteReceive(float ActiveTime)
{
	NewReceiveBox->ActivateVolume(ActiveTime);
}

void ABalloonKeepUpCharacter::OnChargeStarted(ECharacterState State)
{
	Server_RequestStartCharge(State);
	bIsCharging = true;
}

void ABalloonKeepUpCharacter::Server_RequestStartCharge_Implementation(ECharacterState State)
{
	if (!HasAuthority() || bIsCharging) return;
	// 조건은 추후 추가
	ReplicatedState = State;
	SetState(ReplicatedState);
	ChargeStartTime = GetWorld()->GetTimeSeconds();
	ChargeRatio = 0;
}

void ABalloonKeepUpCharacter::OnChargeCompleted()
{
	if (!bIsCharging) return;

	bIsCharging = false;

	Server_CommitCharge();
}

void ABalloonKeepUpCharacter::Server_CommitCharge_Implementation()
{
	if (!HasAuthority()) return;

	const float Elapsed = GetWorld()->GetTimeSeconds() - ChargeStartTime;
	ChargeRatio = FMath::Clamp(Elapsed / MaxChargeTime, 0.3f, 1.5f);

	switch (ReplicatedState)
	{
	case ECharacterState::Receive:
		ExecuteReceive(ChargeRatio);
		break;
	case ECharacterState::Spike:
		ExecuteSpike(ChargeRatio);
		break;
	default:
		break;
	}
	
	ChargeRatio = 0.f;
	ReplicatedState = ECharacterState::Idle;
}

void ABalloonKeepUpCharacter::OnChargeCanceled()
{
	bIsCharging = false;
	Server_CancelCharge();
}

void ABalloonKeepUpCharacter::Server_CancelCharge_Implementation()
{
	bIsCharging = false;
	ChargeRatio = 0.f;
	ReplicatedState = ECharacterState::Idle;
	SetState(ReplicatedState);
}

void ABalloonKeepUpCharacter::OnDivePressed()
{
	Server_RequestDive();
}

void ABalloonKeepUpCharacter::Server_RequestDive_Implementation()
{
	if (!HasAuthority()) return;
	ReplicatedState = ECharacterState::Dive;
	SetState(ReplicatedState);
	
	FVector Forward = GetActorForwardVector();
	FVector DiveVel = Forward * DivePower;
	DiveVel.Z = DiveZ;
	
	LaunchCharacter(DiveVel, true, true);
	NewDiveBox->ActivateVolume(20);
}

void ABalloonKeepUpCharacter::OnDiveEnd()
{
	Server_EndDive();
}

void ABalloonKeepUpCharacter::Server_EndDive_Implementation()
{
	if (!HasAuthority()) return;

	ReplicatedState = ECharacterState::Idle;
	SetState(ReplicatedState);
	NewDiveBox->DeactivateVolume();
}

void ABalloonKeepUpCharacter::ProvideFragments_Implementation(FImpulseContext& Context)
{
	if (!HasAuthority()) return;
	
	UImpulseFragment_Charge* Charge = NewObject<UImpulseFragment_Charge>(this);
	Charge->ChargeRatio = ChargeRatio;
	TSubclassOf<UImpulseFragment> Key = Charge->GetClass();
	
	if (Context.ImpulseFragments.Contains(Key))
	{
		ensureMsgf(false, TEXT("Duplicate ImpulseFragment type: %s"), *Key->GetName());
	}
	
	Context.ImpulseFragments.Add(Key, Charge);
}
