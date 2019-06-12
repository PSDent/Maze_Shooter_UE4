// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExpendableItem.h"
#include "HpPotion.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API AHpPotion : public AExpendableItem
{
	GENERATED_BODY()
	
public:
	AHpPotion();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
