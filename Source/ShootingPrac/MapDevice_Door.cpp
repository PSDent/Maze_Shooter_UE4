// Fill out your copyright notice in the Description page of Project Settings.

#include "MapDevice_Door.h"

AMapDevice_Door::AMapDevice_Door() : Super()
{
	deviceName = "Device_Door";
	interactiveString = "'" + deviceName + "'" + "\nPress 'F' To Activate Device";
	changeSpeed = 0.01f;
	changeDeltaHeight = 5.0f;
	hp = 1000.0f;

	Tags.Add("Coverable");
}


void AMapDevice_Door::ActiveDevice()
{
	bActivated = !bActivated;
	if (bActivated)
	{
		Up();
	}
	else
	{
		Down();
	}
}

void AMapDevice_Door::BeginPlay()
{
	Super::BeginPlay();

	GetActorBounds(false, origin, meshBounds);

	FString CoordinateString = FString::Printf(TEXT("Bound is %s"), *meshBounds.ToCompactString());
	closePos = origin;
	closePos.Z += meshBounds.Z - 20.0f;
	openPos = origin;
	openPos.Z = origin.Z - meshBounds.Z;
}

void AMapDevice_Door::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapDevice_Door::Up()
{
	fChangeDele.BindLambda([&] 
	{
		if (GetActorLocation().Z >= closePos.Z)
		{
			FVector location = GetActorLocation();
			location.Z = closePos.Z;
			SetActorLocation(location);
			return;
		}
		FVector location = GetActorLocation();
		location.Z += changeDeltaHeight;
		SetActorLocation(location);
	}
	);
	GetWorld()->GetTimerManager().SetTimer(hTimer, fChangeDele, changeSpeed, true, 0.0f);
}

void AMapDevice_Door::Down()
{
	fChangeDele.BindLambda([&]
	{
		if (GetActorLocation().Z <= openPos.Z)
		{
			FVector location = GetActorLocation();
			location.Z = openPos.Z;
			SetActorLocation(location);
			return;
		}
		FVector location = GetActorLocation();
		location.Z -= changeDeltaHeight;
		SetActorLocation(location);
	}
	);
	GetWorld()->GetTimerManager().SetTimer(hTimer, fChangeDele, changeSpeed, true, 0.0f);
}