// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapDevice.h"
#include "MapDevice_Door.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API AMapDevice_Door : public AMapDevice
{
	GENERATED_BODY()

public:
	AMapDevice_Door();

	virtual void ActiveDevice();

private:
	float changeSpeed;
	float changeDeltaHeight;
	float nextHeight;
	float hp;

	FVector origin;
	FVector meshBounds;
	FVector closePos;
	FVector openPos;

	FTimerHandle hTimer;
	FTimerDelegate fChangeDele;

private:
	void Down();
	void Up();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
