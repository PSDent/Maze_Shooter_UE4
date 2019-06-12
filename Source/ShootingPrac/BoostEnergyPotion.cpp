// Fill out your copyright notice in the Description page of Project Settings.

#include "BoostEnergyPotion.h"

ABoostEnergyPotion::ABoostEnergyPotion() : AExpendableItem()
{
	boostEnergy = 30.0f;

	Tags.Add("WEnergy_Potion");
}

void ABoostEnergyPotion::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Potion BeginPlay");

	collider->OnComponentBeginOverlap.AddDynamic(this, &ABoostEnergyPotion::OnOverlapBegin);
}

void ABoostEnergyPotion::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

void ABoostEnergyPotion::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Expendablr_Overlap");
	if (!OtherActor->ActorHasTag("Player")) return;
	((AShootingPracCharacter*)OtherActor)->AddBoostEnergy(boostEnergy);

	Destroy();
}
