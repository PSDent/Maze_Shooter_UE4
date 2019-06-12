// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyHelperClass.h"
#include "LaserClass.h"
#include "Components/BoxComponent.h"
#include "Engine.h"
#include "AI_Character.h"
#include "ShootingPracCharacter.h"
#include "Construction.generated.h"

class AAI_Character;
class AShootingPracCharacter;

UCLASS()
class SHOOTINGPRAC_API AConstruction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConstruction();

	void ReceiveDamage(float dmg);
	void SetCoverer(AAI_Character *owner);
	FString GetInteractiveString();

protected:
	void NotifyDestory();

protected:

	UPROPERTY(EditAnywhere)
		float hp;

	UPROPERTY(EditAnywhere)
		TArray<FName> tagArray;

	UBoxComponent *boxCollider;
	AActor *coveringActor;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
