	// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShootingPracGameMode.h"
#include "UObject/ConstructorHelpers.h"

AShootingPracGameMode::AShootingPracGameMode()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<AHUD> hudBP(TEXT("/Game/ThirdPersonCPP/Blueprints/MyCharacterHUD"));
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/Character/ThirdPersonCharacter"));
	static ConstructorHelpers::FClassFinder<UUserWidget> failWidgetObj(TEXT("/Game/ThirdPersonCPP/Blueprints/Character/FailWidget_BP"));
	static ConstructorHelpers::FClassFinder<UUserWidget> winWidgetObj(TEXT("/Game/ThirdPersonCPP/Blueprints/Character/WinWidget_BP"));

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	if (failWidgetObj.Succeeded())
	{
		failWidget = failWidgetObj.Class;
	}
	if (winWidgetObj.Succeeded())
		winWidget = winWidgetObj.Class;

	if (failWidget)
	{
		failWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), failWidget);
	}
	if (winWidget)
	{
		winWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), winWidget);
	}


	// 여기다가 게임 규칙 작성할것
}

void AShootingPracGameMode::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AShootingPracCharacter> actorIter(GetWorld()); actorIter; ++actorIter)
		player = *actorIter;

	for (TActorIterator<AMapDevice_Exit> actorIter(GetWorld()); actorIter; ++actorIter)
		exitArray.Add(*actorIter);
}

void AShootingPracGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckGameOver();
}

void AShootingPracGameMode::CheckGameOver()
{
	if (isEnd) return;

	if (player->GetPlayerDieStatus())
	{
		Cast<APlayerController>(player->GetController())->SetInputMode(FInputModeUIOnly());

		isEnd = true;
		ReleaseAllTimer();

		failWidgetInstance->AddToViewport();

	}
	else if(player->GetPlayerArriveStatus() || !IsRemaining() || IsClear())
	{
		Cast<APlayerController>(player->GetController())->SetInputMode(FInputModeUIOnly());

		isEnd = true;
		ReleaseAllTimer();

		winWidgetInstance->AddToViewport();
	}
}

void AShootingPracGameMode::ReleaseAllTimer()
{
	for (TActorIterator<AAI_Character> actorIter(GetWorld()); actorIter; ++actorIter)
	{
		actorIter->ReleaseTimer();
	}
	player->ReleaseTimer();
}

bool AShootingPracGameMode::IsRemaining()
{
	for (TActorIterator<AAI_Character> actorIter(GetWorld()); actorIter; ++actorIter)
	{
		return true;
	}
	return false;
}

bool AShootingPracGameMode::IsClear()
{
	for (int i = 0; i < exitArray.Num(); ++i)
	{
		if (exitArray[i]->GetClearStatus())
			return true;
	}
	return false;
}