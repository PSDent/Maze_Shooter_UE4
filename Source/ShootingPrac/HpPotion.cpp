// Fill out your copyright notice in the Description page of Project Settings.

#include "HpPotion.h"

AHpPotion::AHpPotion() : AExpendableItem()
{
	hp = 30.0f;

	Tags.Add("WEnergy_Potion");

}

void AHpPotion::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Potion BeginPlay");

	collider->OnComponentBeginOverlap.AddDynamic(this, &AHpPotion::OnOverlapBegin);
}

void AHpPotion::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

void AHpPotion::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag("Player")) return;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "HP_Potion_Overlap");
	((AShootingPracCharacter*)OtherActor)->AddHealthPoint(hp);

	Destroy();
}