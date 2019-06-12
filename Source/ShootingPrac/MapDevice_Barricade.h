// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapDevice.h"
#include "MapDevice_Barricade.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API AMapDevice_Barricade : public AMapDevice
{
	GENERATED_BODY()

public:
	AMapDevice_Barricade();

protected:
	void BeginPlay() override;
	void Tick(float) override;
	void CheckDestroy();

protected:
	float hp;


private:

};
