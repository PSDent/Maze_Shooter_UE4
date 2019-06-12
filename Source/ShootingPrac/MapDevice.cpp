// Fill out your copyright notice in the Description page of Project Settings.

#include "MapDevice.h"

// Sets default values
AMapDevice::AMapDevice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	deviceName = "Device";
	interactiveString = "'" + deviceName + "'" + "\nPress 'F' To Activate Device";
	bActivated = false;

	Tags.Add("Hitable");
	Tags.Add("Construction");
	Tags.Add("MapDevice");
	Tags.Add("Interactable");
	Tags.Add("Wall");
}

// Called when the game starts or when spawned
void AMapDevice::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMapDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapDevice::ActiveDevice()
{
}

void AMapDevice::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

FString AMapDevice::GetInteractiveString()
{
	return interactiveString;
}