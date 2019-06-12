// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapDevice.h"
#include "MapDevice_Elevator.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API AMapDevice_Elevator : public AMapDevice
{
	GENERATED_BODY()

public:
	AMapDevice_Elevator();

	UPROPERTY(EditAnywhere)
		AActor *startPos;

	UPROPERTY(EditAnywhere)
		AActor *endPos;

public:
	void ActiveDevice() override;

private:
	FTimerHandle hTimer;
	FTimerDelegate fChangeDele;

	FVector sPos;
	FVector ePos;
	FVector nowPos;

	float alpha;
	float speed;
	float alphaDelta;
	
private:
	void Up();
	void Down();

protected:
	void BeginPlay() override;

	//UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};