// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Public/CanvasItem.h"
#include "Engine/Canvas.h"
#include "CharacterHUD.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API ACharacterHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
		UTexture2D *crossHairTexture;

public:
	virtual void DrawHUD() override;

};
