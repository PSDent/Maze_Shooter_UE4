// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponEnergyPotion.h"

AWeaponEnergyPotion::AWeaponEnergyPotion() : AExpendableItem()
{
	weaponEnergy = 30.0f;

	Tags.Add("WEnergy_Potion");
}

void AWeaponEnergyPotion::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Potion BeginPlay");

	collider->OnComponentBeginOverlap.AddDynamic(this, &AWeaponEnergyPotion::OnOverlapBegin);
}

void AWeaponEnergyPotion::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

void AWeaponEnergyPotion::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Expendablr_Overlap");
	((AShootingPracCharacter*)OtherActor)->AddWeaponEnergy(weaponEnergy);

	Destroy();
}