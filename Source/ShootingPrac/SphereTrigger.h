// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "SphereTrigger.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API ASphereTrigger : public ATrigger
{
	GENERATED_BODY()
	
public:
	ASphereTrigger();

	void BeginPlay() override;
	void SetRadius(float rad);

private:

protected:

};
