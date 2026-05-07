// Fill out your copyright notice in the Description page of Project Settings.


#include "Balloon/BalloonSpawnPoint.h"

// Sets default values
ABalloonSpawnPoint::ABalloonSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

FVector ABalloonSpawnPoint::GetSpawnLocation()
{
	return GetActorLocation();	
}
