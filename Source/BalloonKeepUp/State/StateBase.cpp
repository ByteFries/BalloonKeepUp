// Fill out your copyright notice in the Description page of Project Settings.


#include "StateBase.h"

#include "EnhancedInputComponent.h"

void UStateBase::HandleInput(const EInputAction Action, const ETriggerEvent Event)
{
	switch (Event)
	{
	case ETriggerEvent::Started:
		HandlePressed(Action);
		break;
	case ETriggerEvent::Triggered:
		HandleTriggered(Action);
		break;
	case ETriggerEvent::Completed:
		HandleReleased(Action);
		break;
	case ETriggerEvent::Canceled:
		HandleTriggered(Action);
		break;
	default:
		break;
	}
}
