// Fill out your copyright notice in the Description page of Project Settings.

#include "TrapObject_Ball.h"

ATrapObject_Ball::ATrapObject_Ball() : ATrapObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATrapObject_Ball::BeginPlay()
{
	Super::BeginPlay();
	((USphereComponent*)RootComponent)->OnComponentBeginOverlap.AddDynamic(this, &ATrapObject_Ball::OnOverlapBegin);
}

// Called every frame
void ATrapObject_Ball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATrapObject_Ball::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "ASD");
	
	if (OtherActor->IsA(AShootingPracCharacter::StaticClass()))
	{
		Cast<AShootingPracCharacter>(OtherActor)->ReceiveDamage(damage);
	}
	else if (OtherActor->IsA(AAI_Character::StaticClass()))
	{
		Cast<AAI_Character>(OtherActor)->ReceiveDamage(damage);
	}
}