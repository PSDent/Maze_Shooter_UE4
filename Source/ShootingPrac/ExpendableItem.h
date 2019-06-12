// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Engine.h"
#include "Components/CapsuleComponent.h"
#include "ShootingPracCharacter.h"
#include "ExpendableItem.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API AExpendableItem : public AItem
{
	GENERATED_BODY()

public:
	AExpendableItem();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void ActiveItem() override;
	//void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void Floating(float delta);

protected:
	UCapsuleComponent *collider;

	float hp;
	float boostEnergy;
	float weaponEnergy;

	// 기타 여러 부가 효과 

private:
	
};
