// Fill out your copyright notice in the Description page of Project Settings.

#include "SphereTrigger.h"

ASphereTrigger::ASphereTrigger() : Super()
{
	USphereComponent *collider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	collider->InitSphereRadius(20.0f);
	collider->BodyInstance.SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = collider;
}

void ASphereTrigger::BeginPlay()
{
	Super::BeginPlay();
}

void ASphereTrigger::SetRadius(float rad)
{
	Cast<USphereComponent>(RootComponent)->InitSphereRadius(rad);
}