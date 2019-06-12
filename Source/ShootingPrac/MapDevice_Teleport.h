// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapDevice.h"
#include "Components/CapsuleComponent.h"
#include "Engine.h"
#include "MyHelperClass.h"
#include "MapDevice_Teleport.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API AMapDevice_Teleport : public AMapDevice
{
	GENERATED_BODY()

public:
	AMapDevice_Teleport();

	TArray<AActor*>* GetHumanList();

public:
	UPROPERTY(EditAnywhere)
		AActor *otherTeleportActor;

private:
	AMapDevice_Teleport *otherTeleport;
	UCapsuleComponent* collider;
	FVector otherTeleportLocation;
	TArray<AActor*> humanList;

private:
	void BeginPlay() override;
	void Tick(float deltaTime) override;
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	
};
