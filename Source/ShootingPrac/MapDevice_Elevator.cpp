// Fill out your copyright notice in the Description page of Project Settings.

#include "MapDevice_Elevator.h"

AMapDevice_Elevator::AMapDevice_Elevator() : Super()
{
	deviceName = "Elevator";
	interactiveString = "'" + deviceName + "'" + "\nPress 'F' To Activate Device";

	alpha = 0;
	speed = 0.001f;
	alphaDelta = 0.001f;
	bActivated = true;
}

void AMapDevice_Elevator::BeginPlay()
{
	Super::BeginPlay();

	if (startPos)
	{
		sPos = startPos->GetActorLocation();
		SetActorLocation(sPos);
	}
	if(endPos)
		ePos = endPos->GetActorLocation();


}

void AMapDevice_Elevator::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AMapDevice_Elevator::Up()
{
	fChangeDele.BindLambda([&]
	{
		alpha += alphaDelta;
		nowPos = FMath::Lerp(sPos, ePos, alpha);
		SetActorLocation(nowPos);
		if (alpha >= 1.0f)
			GetWorld()->GetTimerManager().ClearTimer(hTimer);
	}
	);
	GetWorld()->GetTimerManager().SetTimer(hTimer, fChangeDele, speed, true, 0.0f);
}

void AMapDevice_Elevator::Down()
{
	fChangeDele.BindLambda([&]
	{
		alpha -= alphaDelta;
		nowPos = FMath::Lerp(sPos, ePos, alpha);
		SetActorLocation(nowPos);
		if (alpha <= 0.0f)
			GetWorld()->GetTimerManager().ClearTimer(hTimer);
	}
	);
	GetWorld()->GetTimerManager().SetTimer(hTimer, fChangeDele, speed, true, 0.0f);
}

void AMapDevice_Elevator::ActiveDevice()
{
	if (!bActivated)
	{
		Up();
	}
	else
	{
		Down();
	}
	bActivated = !bActivated;
}