// Fill out your copyright notice in the Description page of Project Settings.

#include "ExpendableItem.h"

AExpendableItem::AExpendableItem() : AItem()
{
	SetActorTickEnabled(true);
	PrimaryActorTick.bCanEverTick = true;
}

void AExpendableItem::ActiveItem()
{

}

//void AExpendableItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//
//}

void AExpendableItem::BeginPlay()
{
	Super::BeginPlay();

	TArray<USceneComponent*> arr;
	RootComponent->GetChildrenComponents(false, arr);

	for (int i = 0; i < arr.Num(); ++i)
	{
		if (arr[i]->GetName() == "Collider")
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Expendable_COLL");
			collider = (UCapsuleComponent*)arr[i];
			break;
		}
	}
}

void AExpendableItem::Floating(float delta)
{
	FVector newLocation = GetActorLocation();
	static float sum = 0;
	float deltaHeight;

	deltaHeight = FMath::Sin(sum + delta) - FMath::Sin(sum);
	newLocation.Z += deltaHeight * 20;
	SetActorLocation(newLocation);
	sum += delta;
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Expendable");
	
}

void AExpendableItem::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	Floating(deltaTime);
}