// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "ShootingPracCharacter.h"
#include "AI_Character.h"
#include "MapDevice_Exit.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "ShootingPracGameMode.generated.h"

class AShootingPracCharacter;
class AAI_Character;

UCLASS(minimalapi)
class AShootingPracGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShootingPracGameMode();
	
	void Tick(float DeltaTime) override;
	void BeginPlay() override;

private:
	bool isEnd = false;

private:
	void CheckGameOver();
	void ReleaseAllTimer();
	bool IsClear();
	bool IsRemaining();

private:
	
	TArray<AMapDevice_Exit*> exitArray;
	AShootingPracCharacter *player;

	TSubclassOf<UUserWidget> failWidget;
	TSubclassOf<UUserWidget> winWidget;

	UPROPERTY()
		UUserWidget *failWidgetInstance;

	UPROPERTY()
		UUserWidget *winWidgetInstance;

};



