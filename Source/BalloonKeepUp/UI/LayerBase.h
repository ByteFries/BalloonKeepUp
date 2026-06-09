// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LayerBase.generated.h"

class UBorder;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BALLOONKEEPUP_API ULayerBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="UI|Layer")
	UUserWidget* PushWidget(const TSubclassOf<UUserWidget>& WidgetClass);

	UFUNCTION(BlueprintCallable, Category="UI|Layer")
	void PopWidget();

	UFUNCTION(BlueprintCallable, Category="UI|Layer")
	void ClearStack();

	UFUNCTION(BlueprintCallable, Category="UI|Layer")
	UUserWidget* GetTopWidget();

protected:
	UPROPERTY(BlueprintReadOnly, Category="UI|Layer")
	TArray<TObjectPtr<UUserWidget>> WidgetStack;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UBorder> Border;
};
