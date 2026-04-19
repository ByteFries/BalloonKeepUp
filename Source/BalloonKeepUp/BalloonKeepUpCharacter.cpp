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
#include "State/StateBase.h"
#include "Net/UnrealNetwork.h"
#include "Physics/Impulse/ImpulseBoxComponent.h"
#include "Physics/Impulse/Fragment/ImpulseFragment_Charge.h"
#include "State/StateMachineComponent.h"
#include "State/State_Charge.h"
#include "State/State_Dive.h"
#include "State/State_Idle.h"

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


void ABalloonKeepUpCharacter::RequestDiveAction()
{
	
}

void ABalloonKeepUpCharacter::RequestChargeAction(float ChargeRatio)
{
	if (CurrentState != ECharacterState::Charge || !HasAuthority()) return;
	
	switch (ReplicatedChargeAction)
	{
	case EChargeAction::Receive:
		NewReceiveBox->ActivateVolume(0.5f);
		break;
	case EChargeAction::Spike:
		SpikeBox->ActivateVolume(0.5);
		break;
	default:
		break;
	}
}

void ABalloonKeepUpCharacter::BeginPlay()
{
	Super::BeginPlay();
	States.Empty();

	States.Add(ECharacterState::Idle, NewObject<UState_Idle>(this));
	States.Add(ECharacterState::Dive, NewObject<UState_Dive>(this));
	States.Add(ECharacterState::Charge, NewObject<UState_Charge>(this));
}

void ABalloonKeepUpCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Look);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Look);
		
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Started, this, &ABalloonKeepUpCharacter::OnChargeStarted, EChargeAction::Spike);
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Completed, this, &ABalloonKeepUpCharacter::OnChargeCompleted);
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Canceled, this, &ABalloonKeepUpCharacter::OnChargeCanceled);

		EnhancedInputComponent->BindAction(ReceiveAction, ETriggerEvent::Started, this, &ABalloonKeepUpCharacter::OnChargeStarted, EChargeAction::Receive);
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
	SetState();
}

void ABalloonKeepUpCharacter::SetState()
{
	if (States.Find(CurrentState))
	{
		//if (HasAuthority()) States[CurrentState]->ExitState_Server();

		//States[CurrentState]->EnterState_Client();
	}

	CurrentState = ReplicatedState;
	
	if (!States.Find(CurrentState))
	{
		ensureMsgf(false, TEXT("Can't Find State"));
		return;
	}
	
	//if (HasAuthority()) States[CurrentState]->EnterState_Server();

	//States[CurrentState]->EnterState_Client();
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
	// signal the character to jump
	Jump();
}

void ABalloonKeepUpCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ABalloonKeepUpCharacter::OnChargeStarted(EChargeAction Action)
{
	if (CurrentState == ECharacterState::Charge) return;
	
	Server_RequestStartCharge(Action);
}

void ABalloonKeepUpCharacter::Server_RequestStartCharge_Implementation(EChargeAction Action)
{
	if (!HasAuthority()) return;
	// 조건은 추후 추가
	ReplicatedState = ECharacterState::Charge;
	ReplicatedChargeAction = Action;
	SetState();
}

void ABalloonKeepUpCharacter::OnChargeCompleted()
{
	if (CurrentState != ECharacterState::Charge) return;

	Server_CommitCharge();
}

void ABalloonKeepUpCharacter::Server_CommitCharge_Implementation()
{
	if (!HasAuthority()) return;
	
	UState_Charge* State = Cast<UState_Charge>(States[CurrentState]);
	if (!State)
	{
		ensureMsgf(false, TEXT("Not Charge State"));
		return;
	}
	//State->EnterState_Server();
	ReplicatedState = ECharacterState::Idle;
}

void ABalloonKeepUpCharacter::OnChargeCanceled()
{
	Server_CancelCharge();
}

void ABalloonKeepUpCharacter::Server_CancelCharge_Implementation()
{
	ReplicatedState = ECharacterState::Idle;
	SetState();
}

void ABalloonKeepUpCharacter::OnDivePressed()
{
	Server_RequestDive();
}

void ABalloonKeepUpCharacter::Server_RequestDive_Implementation()
{
	if (!HasAuthority()) return;
	ReplicatedState = ECharacterState::Dive;
	SetState();
	
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
	SetState();
	NewDiveBox->DeactivateVolume();
}

void ABalloonKeepUpCharacter::ProvideFragments_Implementation(FImpulseContext& Context)
{
	if (!HasAuthority()) return;
	
	UImpulseFragment_Charge* Charge = NewObject<UImpulseFragment_Charge>(this);
	//Charge->ChargeRatio = ChargeRatio;
	TSubclassOf<UImpulseFragment> Key = Charge->GetClass();
	
	if (Context.ImpulseFragments.Contains(Key))
	{
		ensureMsgf(false, TEXT("Duplicate ImpulseFragment type: %s"), *Key->GetName());
	}
	
	Context.ImpulseFragments.Add(Key, Charge);
}
