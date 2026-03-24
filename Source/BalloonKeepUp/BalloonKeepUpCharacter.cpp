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
#include "Physics/Impulse/ImpulseBoxComponent.h"

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

	//SpikeBox = CreateDefaultSubobject<UImpulseBoxComponent>(TEXT("SpikeBox"));
	//SpikeBox->SetupAttachment(RootComponent);
	
	NewSpikeBox = CreateDefaultSubobject<UImpulseBoxComponent>(TEXT("NewSpikeBox"));
	NewSpikeBox->SetupAttachment(RootComponent);
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ABalloonKeepUpCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::Look);


		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Started, this, &ABalloonKeepUpCharacter::OnSpikeStarted);
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Triggered, this, &ABalloonKeepUpCharacter::OnSpikeTriggered);
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Completed, this, &ABalloonKeepUpCharacter::OnSpikeCompleted);
		EnhancedInputComponent->BindAction(SpikeAction, ETriggerEvent::Canceled, this, &ABalloonKeepUpCharacter::OnSpikeCanceled);
	}
	else
	{
		UE_LOG(LogBalloonKeepUp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
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

void ABalloonKeepUpCharacter::OnSpikeStarted()
{
	bIsCharging = true;
	ChargeStartTime = GetWorld()->GetTimeSeconds();
	CurrentChargeRatio = 0;
}

void ABalloonKeepUpCharacter::OnSpikeTriggered()
{
	if (!bIsCharging) return;

	const float Elapsed = GetWorld()->GetTimeSeconds() - ChargeStartTime;
	CurrentChargeRatio = FMath::Clamp(Elapsed / MaxSpikeChargeTime, 0.f, 1.f);
}

void ABalloonKeepUpCharacter::OnSpikeCompleted()
{
	if (!bIsCharging) return;

	bIsCharging = false;

	const float FinalChargeRatio = CurrentChargeRatio;
	
	ExecuteSpike(FinalChargeRatio);
	
	CurrentChargeRatio = 0.f;
}

void ABalloonKeepUpCharacter::OnSpikeCanceled()
{
	bIsCharging = false;
	CurrentChargeRatio = 0.f;
}

void ABalloonKeepUpCharacter::ExecuteSpike(float ChargeRatio)
{
	NewSpikeBox->ActivateVolume(0.5f);
}

