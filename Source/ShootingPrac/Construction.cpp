// Fill out your copyright notice in the Description page of Project Settings.

#include "Construction.h"

// Sets default values
AConstruction::AConstruction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("Hitable");
	Tags.Add("Coverable");
	Tags.Add("Construction");

	hp = 100.0f;
	coveringActor = nullptr;
}

// Called when the game starts or when spawned
void AConstruction::BeginPlay()
{
	Super::BeginPlay();

	USceneComponent *staticMesh = UMyHelperClass::GetChildComponentByName<USceneComponent>(RootComponent, "StaticMesh");
	boxCollider = (UBoxComponent*)UMyHelperClass::GetChildComponentByName<USceneComponent>(staticMesh, "Collider");
	//boxCollider->OnComponentBeginOverlap.AddDynamic(this, &AConstruction::OnOverlapBegin);
	boxCollider->OnComponentHit.AddDynamic(this, &AConstruction::OnHit);

	for (int i = 0; i < tagArray.Num(); ++i)
	{
		Tags.Add(tagArray[i]);
	}

}

// Called every frame
void AConstruction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AConstruction::ReceiveDamage(float dmg)
{
	hp -= dmg;
	if (hp <= 0)
	{
		if (coveringActor)
		{
			NotifyDestory();
		}
		else
		{
			Destroy();
		}
	}
}

void AConstruction::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Projectile"))
	{
		float dmg = ((ALaserClass*)OtherActor)->GetDamage();
		ReceiveDamage(dmg);
	}
}

void AConstruction::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "AHHAHAHHA");
}

void AConstruction::SetCoverer(AAI_Character *owner)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Setcoverer");

	if(owner)
		coveringActor = owner;
}

void AConstruction::NotifyDestory()
{

	if (coveringActor->IsA(AAI_Character::StaticClass()))
		Cast<AAI_Character>(coveringActor)->DetectCoverDestroy();
	else if (coveringActor->IsA(AShootingPracCharacter::StaticClass()))
		Cast<AShootingPracCharacter>(coveringActor)->DetectCoverDestroy();

}

FString AConstruction::GetInteractiveString()
{
	return "Press 'Ctrl' To Cover at Construction";
}