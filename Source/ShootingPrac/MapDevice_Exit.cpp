// Fill out your copyright notice in the Description page of Project Settings.

#include "MapDevice_Exit.h"

AMapDevice_Exit::AMapDevice_Exit() : Super()
{

}

void AMapDevice_Exit::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add("Exit");

	collider = UMyHelperClass::GetChildComponentByName<UCapsuleComponent>(RootComponent, "Collider");
	collider->OnComponentBeginOverlap.AddDynamic(this, &AMapDevice_Exit::OnOverlapBegin);

}

void AMapDevice_Exit::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		isClear = true;
	}
}

bool AMapDevice_Exit::GetClearStatus()
{
	return isClear;
}