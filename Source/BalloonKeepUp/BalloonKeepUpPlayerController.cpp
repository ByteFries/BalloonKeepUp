// Copyright Epic Games, Inc. All Rights Reserved.


#include "BalloonKeepUpPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "BalloonKeepUp.h"
#include "GameFramework/BalloonRelayGameMode.h"
#include "GameFramework/BalloonRelayGameState.h"
#include "GameFramework/BalloonKeepUpPlayerState.h"
#include "Widgets/Input/SVirtualJoystick.h"

static FString NetModeToStr(UWorld* World)
{
	if (!World) return TEXT("NoWorld");
	switch (World->GetNetMode())
	{
	case NM_Standalone: return TEXT("Standalone");
	case NM_DedicatedServer: return TEXT("DedicatedServer");
	case NM_ListenServer: return TEXT("ListenServer");
	case NM_Client: return TEXT("Client");
	default: return TEXT("Unknown");
	}
}

#define PCLOG(FuncName) \
UE_LOG(LogTemp, Warning, TEXT("[%s] %s | NetMode=%s | Local=%d | Role=%s | PC=%s Pawn=%s"), \
TEXT(FuncName), \
*GetName(), \
*NetModeToStr(GetWorld()), \
IsLocalController() ? 1 : 0, \
*UEnum::GetValueAsString(GetLocalRole()), \
*GetNameSafe(this), \
*GetNameSafe(GetPawn()))

void ABalloonKeepUpPlayerController::BeginPlay()
{
	Super::BeginPlay();


	PCLOG("BeginPlay");
	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogBalloonKeepUp, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}

	if (!IsLocalPlayerController()) return;

	ApplyFreeze(true);

	if (ABalloonRelayGameState* GS = GetWorld()->GetGameState<ABalloonRelayGameState>())
	{
		GS->OnPlayEnableChanged.AddUObject(this, &ABalloonKeepUpPlayerController::HandlePlayEnableChanged);
	}

	TryReady();
}

void ABalloonKeepUpPlayerController::SetIgnoreMoveInput(bool bNewMoveInput)
{
	Super::SetIgnoreMoveInput(bNewMoveInput);
	
	//FDebug::DumpStackTraceToLog(ELogVerbosity::Warning);
}

void ABalloonKeepUpPlayerController::HandlePlayEnableChanged(bool bPlayEnabled)
{
	ApplyFreeze(!bPlayEnabled);
}

void ABalloonKeepUpPlayerController::ApplyFreeze(bool bFreeze)
{
	SetIgnoreMoveInput(bFreeze);
}

void ABalloonKeepUpPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//PCLOG("OnPossess");
	
	TryReady();
}

void ABalloonKeepUpPlayerController::TryReady()
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	if (MyPawn->GetController() != this) return;

	ABalloonKeepUpPlayerState* PS = GetPlayerState<ABalloonKeepUpPlayerState>();
	if (!PS) return;
	
	if (!PS->IsReady())
	{
		//UE_LOG(LogTemp, Warning, TEXT("ReportReady"));
		Server_ReportReady();
	}
}

void ABalloonKeepUpPlayerController::Server_ReportReady_Implementation()
{
	APawn* P = GetPawn();
	if (!P) return;

	if (P->GetController() != this) return;

	ABalloonKeepUpPlayerState* PS = GetPlayerState<ABalloonKeepUpPlayerState>();
	if (!PS) return;

	PS->SetReady(true);

	if (ABalloonRelayGameMode* GM = GetWorld()->GetAuthGameMode<ABalloonRelayGameMode>())
	{
		GM->CheckAllPlayersReady();
	}
}

void ABalloonKeepUpPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void ABalloonKeepUpPlayerController::ClientRestart_Implementation(APawn* NewPawn)
{
	Super::ClientRestart_Implementation(NewPawn);
	if (ABalloonRelayGameState* GS = GetWorld()->GetGameState<ABalloonRelayGameState>())
	HandlePlayEnableChanged(GS->GetPlayEnabled());
	//PCLOG("ClientRestart");
	TryReady();
}


void ABalloonKeepUpPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();
	//PCLOG("BeginPlayingState");
}
