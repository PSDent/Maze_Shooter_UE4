// Fill out your copyright notice in the Description page of Project Settings.

#include "TrapObject.h"

// Sets default values
ATrapObject::ATrapObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("TrapObject");
	Tags.Add("Construction");
}

// Called when the game starts or when spawned
void ATrapObject::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATrapObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATrapObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

