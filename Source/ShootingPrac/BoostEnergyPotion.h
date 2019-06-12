// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExpendableItem.h"
#include "BoostEnergyPotion.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API ABoostEnergyPotion : public AExpendableItem
{
	GENERATED_BODY()

public:
	ABoostEnergyPotion();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

};
