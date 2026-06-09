// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainHUD.h"

#include "Blueprint/UserWidget.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
	if (PrimaryLayerClass)
	{
		if (UUserWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayerController(), PrimaryLayerClass))
		{
			Widget->AddToViewport();
		}
	}

}

ULayerBase* AMainHUD::GetLayer(const FGameplayTag& Tag)
{
	return BaseLayers.FindRef(Tag);
}

void AMainHUD::Initialize()
{
	//for ()
	{
		
	}
}
