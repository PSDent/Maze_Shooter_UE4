// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrapObject.h"
#include "AI_Character.h"
#include "ShootingPracCharacter.h"
#include "Components/SphereComponent.h"
#include "TrapObject_Ball.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API ATrapObject_Ball : public ATrapObject
{
	GENERATED_BODY()

public:
	ATrapObject_Ball();

protected:

	float damage = 300.0f;

private:

	void Tick(float deltatime) override;
	void BeginPlay() override;
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
};
