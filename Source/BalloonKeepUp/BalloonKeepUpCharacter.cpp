// Copyright Epic Games, Inc. All Rights Reserved.

#include "BalloonKeepUpCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "BalloonKeepUp.h"
#include "State/StateBase.h"
#include "Net/UnrealNetwork.h"
#include "Physics/Impulse/ImpulseBoxComponent.h"
#include "Physics/Impulse/Fragment/ImpulseFragment_Charge.h"
#include "State/StateMachineComponent.h"
#include "State/State_Idle.h"
#include "State/State_Receive.h"
#include "State/State_Spike.h"

ABalloonKeepUpCharacter::ABalloonKeepUpCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	StateMachine = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachine"));
	
	SpikeBox = CreateDefaultSubobject<UImpulseBoxComponent>(TEXT("SpikeBox"));
	SpikeBox->SetupAttachment(RootComponent);

	NewReceiveBox = CreateDefaultSubobject<UImpulseBoxComponent>(TEXT("NewReceiveBox"));
	NewReceiveBox->SetupAttachment(RootComponent);

	NewDiveBox = CreateDefaultSubobject<UImpulseBoxComponent>(TEXT("NewDiveBox"));
	NewDiveBox->SetupAttachment(RootComponent);

	bReplicates = true;
}

void ABalloonKeepUpCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABalloonKeepUpCharacter::RequestChargeAction(const EInputAction Action, const float InChargeRatio)
{
	if (!HasAuthority()) return;
	
	ChargeRatio = InChargeRatio;

	switch (Action)
	{
	case EInputAction::Receive:
		StateMachine->RequestTransition(UState_Receive::StaticClass());
		break;
	case EInputAction::Spike:
		StateMachine->RequestTransition(UState_Spike::StaticClass());
		break;
	default:
		break;
	}
}



void ABalloonKeepUpCharacter::EnableImpulseBox(const EImpulseBoxType Type, const float ActiveTime)
{
	if (!HasAuthority()) return;

	switch (Type)
	{
	case EImpulseBoxType::Dive:
		break;
	case EImpulseBoxType::Spike:
		SpikeBox->ActivateVolume(ActiveTime);
		break;
	case EImpulseBoxType::Receive:
		NewReceiveBox->ActivateVolume(ActiveTime);
		break;
	}
}

void ABalloonKeepUpCharacter::DeactivateImpulseBox(const EImpulseBoxType Type)
{
	if (!HasAuthority()) return;

	switch (Type)
	{
	case EImpulseBoxType::Dive:
		break;
	case EImpulseBoxType::Spike:
		SpikeBox->DeactivateVolume();
		break;
	case EImpulseBoxType::Receive:
		NewReceiveBox->DeactivateVolume();
		break;
	}
}

void ABalloonKeepUpCharacter::RequestHandleInput_Implementation(const EInputAction Action, const ETriggerEvent TriggerEvent)
{
	StateMachine->HandleInput(Action, TriggerEvent);
}

void ABalloonKeepUpCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Look);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Look);
		
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Started, this, &ABalloonKeepUpCharacter::ChargeStart, EInputAction::Spike);
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Completed, this, &ABalloonKeepUpCharacter::ChargeComplete, EInputAction::Spike);
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Canceled, this, &ABalloonKeepUpCharacter::ChargeCancel, EInputAction::Spike);

		EnhancedInputComponent->BindAction(ReceiveAction, ETriggerEvent::Started, this, &ABalloonKeepUpCharacter::ChargeStart, EInputAction::Receive);
		EnhancedInputComponent->BindAction(ReceiveAction, ETriggerEvent::Completed, this, &ABalloonKeepUpCharacter::ChargeComplete, EInputAction::Receive);
		EnhancedInputComponent->BindAction(ReceiveAction, ETriggerEvent::Canceled, this, &ABalloonKeepUpCharacter::ChargeCancel, EInputAction::Receive);

		EnhancedInputComponent->BindAction(DiveAction, ETriggerEvent::Started, this, &ABalloonKeepUpCharacter::DiveStart);
	}
	else
	{
		UE_LOG(LogBalloonKeepUp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABalloonKeepUpCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABalloonKeepUpCharacter, bIsDiving);
}


void ABalloonKeepUpCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (!HasAuthority()) return;

	if (bIsDiving)
	{
		bIsDiving = false;
		ApplyDiveCapsule(false);
		NewDiveBox->DeactivateVolume();

		StateMachine->RequestTransition(UState_Idle::StaticClass());
	}
}

void ABalloonKeepUpCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	DoMove(MovementVector.X, MovementVector.Y);
}

void ABalloonKeepUpCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ABalloonKeepUpCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

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
	Jump();
}

void ABalloonKeepUpCharacter::DoJumpEnd()
{
	StopJumping();
}

void ABalloonKeepUpCharacter::ChargeStart(const EInputAction Action)
{
	RequestHandleInput(Action, ETriggerEvent::Started);
}

void ABalloonKeepUpCharacter::ChargeComplete(const EInputAction Action)
{
	RequestHandleInput(Action, ETriggerEvent::Completed);
}

void ABalloonKeepUpCharacter::ChargeCancel(const EInputAction Action)
{
	RequestHandleInput(Action, ETriggerEvent::Canceled);
}

void ABalloonKeepUpCharacter::DoSpike()
{
	if (!HasAuthority()) return;
	bIsSpiking = true;
	OnSpikeMontageStart();
}

void ABalloonKeepUpCharacter::DoReceive()
{
	if (!HasAuthority()) return;
	bIsReceiving = true;
	OnReceiveMontageStart();
}

void ABalloonKeepUpCharacter::OnRep_Spiking()
{
	if (bIsSpiking)
	{
		OnSpikeMontageStart();
	}
}

void ABalloonKeepUpCharacter::OnRep_Receiving()
{
	if (bIsReceiving)
	{
		OnReceiveMontageStart();
	}
}

void ABalloonKeepUpCharacter::OnSpikeMontageStart()
{
	if (!SpikeMontage)
	{
		ensureMsgf(false, TEXT("No SpikeMontage"));
		return;
	}
	
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	Anim->Montage_Play(SpikeMontage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ABalloonKeepUpCharacter::OnSpikeMontageEnded);

	Anim->Montage_SetEndDelegate(EndDelegate, SpikeMontage);
}

void ABalloonKeepUpCharacter::OnSpikeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!HasAuthority()) return;

	bIsSpiking = false;
	StateMachine->RequestTransition(UState_Idle::StaticClass());
}

void ABalloonKeepUpCharacter::OnReceiveMontageStart()
{
	if (!ReceiveMontage)
	{
		ensureMsgf(false, TEXT("No ReceiveMontage"));
		return;
	}

	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	Anim->Montage_Play(ReceiveMontage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ABalloonKeepUpCharacter::OnReceiveMontageEnded);

	Anim->Montage_SetEndDelegate(EndDelegate, ReceiveMontage);
}

void ABalloonKeepUpCharacter::OnReceiveMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!HasAuthority()) return;

	bIsReceiving = false;
	StateMachine->RequestTransition(UState_Idle::StaticClass());
}

void ABalloonKeepUpCharacter::DiveStart()
{
	RequestHandleInput(EInputAction::Dive, ETriggerEvent::Started);
}

void ABalloonKeepUpCharacter::DoDive(FVector DiveVel)
{
	if (!HasAuthority()) return;
	if (bIsDiving) return;

	bIsDiving = true;
	ApplyDiveCapsule(true);
	
	LaunchCharacter(DiveVel, true, true);
	NewDiveBox->ActivateVolume(20);
}

void ABalloonKeepUpCharacter::CancelDive()
{
	if (!HasAuthority()) return;

	bIsDiving = false;
	ApplyDiveCapsule(false);
	
	NewDiveBox->DeactivateVolume();
	StateMachine->RequestTransition(UState_Idle::StaticClass());
}

void ABalloonKeepUpCharacter::OnRep_Diving()
{
	ApplyDiveCapsule(bIsDiving);
	
	if (bIsDiving) NewDiveBox->ActivateVolume(20);
	else NewDiveBox->DeactivateVolume();
}

void ABalloonKeepUpCharacter::ApplyDiveCapsule(bool IsDiving)
{
	if (IsDiving)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(40.f, true);
	}
	else
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(90.f, true);
	}
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
