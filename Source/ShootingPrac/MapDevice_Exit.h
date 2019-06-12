// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapDevice.h"
#include "Components/CapsuleComponent.h"
#include "MyHelperClass.h"
#include "MapDevice_Exit.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API AMapDevice_Exit : public AMapDevice
{
	GENERATED_BODY()

public:
	AMapDevice_Exit();

	bool GetClearStatus();

private:
	UCapsuleComponent* collider;
	bool isClear = false;

private:
	void BeginPlay() override;
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


protected:

	
};
