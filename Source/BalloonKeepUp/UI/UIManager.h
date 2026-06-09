// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIManager.generated.h"

/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void ShowHUD();
	void HideHUD();

	void ShowCountDown();
	void HideCountDown();

	void ShowPause();
	void HidePause();

private:
	
};
