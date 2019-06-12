// Fill out your copyright notice in the Description page of Project Settings.

#include "MapDevice_Barricade.h"

AMapDevice_Barricade::AMapDevice_Barricade() : Super()
{
	hp = 1000.0f;
}

void AMapDevice_Barricade::BeginPlay()
{
	Super::BeginPlay();
}

void AMapDevice_Barricade::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	CheckDestroy();
}

void AMapDevice_Barricade::CheckDestroy()
{
	if (hp <= 0)
		Destroy();
}