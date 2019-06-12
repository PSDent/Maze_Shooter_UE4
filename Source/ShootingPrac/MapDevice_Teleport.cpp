// Fill out your copyright notice in the Description page of Project Settings.

#include "MapDevice_Teleport.h"

AMapDevice_Teleport::AMapDevice_Teleport() : Super()
{

}

void AMapDevice_Teleport::BeginPlay()
{
	Super::BeginPlay();

	otherTeleport = (AMapDevice_Teleport*)otherTeleportActor;
	collider = UMyHelperClass::GetChildComponentByName<UCapsuleComponent>(RootComponent, "Collider");
	collider->OnComponentBeginOverlap.AddDynamic(this, &AMapDevice_Teleport::OnOverlapBegin);
	collider->OnComponentEndOverlap.AddDynamic(this, &AMapDevice_Teleport::OnOverlapEnd);
	if(otherTeleport)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, otherTeleport->GetName());

}
	  
void AMapDevice_Teleport::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

void AMapDevice_Teleport::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Human"))
	{
		if (humanList.Find(OtherActor) == INDEX_NONE)
		{
			otherTeleport->GetHumanList()->Add(OtherActor);
			OtherActor->SetActorLocation(otherTeleport->GetActorLocation());
		}
	}
}

void AMapDevice_Teleport::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (humanList.Find(OtherActor) != INDEX_NONE)
	{
		humanList.Remove(OtherActor);
	}
}

TArray<AActor*>* AMapDevice_Teleport::GetHumanList()
{
	return &humanList;
}