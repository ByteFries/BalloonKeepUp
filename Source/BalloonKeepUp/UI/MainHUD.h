// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class ULayerBase;
/**
 * 
 */
UCLASS()
class BALLOONKEEPUP_API AMainHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	
	ULayerBase* GetLayer(const FGameplayTag& Tag);

protected:
	void Initialize();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI|HUD")
	TSubclassOf<UUserWidget> PrimaryLayerClass;

	UPROPERTY(BlueprintReadOnly, Category="UI|HUD")
	TMap<FGameplayTag, TObjectPtr<ULayerBase>> BaseLayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI|HUD")
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> InitWidgetClasses;
};
