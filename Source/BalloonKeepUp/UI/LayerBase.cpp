// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayerBase.h"

#include "Components/Border.h"

UUserWidget* ULayerBase::PushWidget(const TSubclassOf<UUserWidget>& WidgetClass)
{
	if (!ensure(Border))
	{
		return nullptr;
	}
	
	if (UUserWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayer(), WidgetClass))
	{
		if (UUserWidget* TopWidget = GetTopWidget())
		{
			TopWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		
		Border->ClearChildren();
		Border->AddChild(Widget);
		
		WidgetStack.Add(Widget);
		Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		
		return Widget;
	}
	
	return nullptr;
}

void ULayerBase::PopWidget()
{
	if (UUserWidget* TopWidget = GetTopWidget())
	{
		TopWidget->RemoveFromParent();
		WidgetStack.Remove(TopWidget);
		Border->ClearChildren();

		if (UUserWidget* NextWidget = GetTopWidget())
		{
			NextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Border->AddChild(NextWidget);
		}
	}
}

void ULayerBase::ClearStack()
{
	for (UUserWidget* Widget : WidgetStack)
	{
		if (IsValid(Widget))
		{
			Widget->RemoveFromParent();
		}
	}

	WidgetStack.Empty();

	if (Border)
	{
		Border->ClearChildren();
	}
}

UUserWidget* ULayerBase::GetTopWidget()
{
	if (WidgetStack.IsEmpty())
	{
		return nullptr;
	}

	return WidgetStack.Last().Get();
}
