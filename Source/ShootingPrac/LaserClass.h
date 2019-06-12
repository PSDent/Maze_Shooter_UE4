// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "LaserClass.generated.h"

UCLASS()
class SHOOTINGPRAC_API ALaserClass : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent *projMovementComponent;

	UParticleSystem *hitParticle;
	UParticleSystem *bulletTrail;

	UPROPERTY(VisibleAnywhere)
		UCapsuleComponent *collision;

public:
	float GetDamage();

private:
	float damage;
public:	
	// Sets default values for this actor's properties
	ALaserClass();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void FireInDir(const FVector &shootDir, float dmg);

};
