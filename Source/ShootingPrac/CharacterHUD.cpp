// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterHUD.h"

void ACharacterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (crossHairTexture)
	{
		FVector2D center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

		FVector2D crossHairDrawPosition(center.X - (crossHairTexture->GetSurfaceWidth() * 0.5f), center.Y - (crossHairTexture->GetSurfaceHeight() * 0.5f));
	
		FCanvasTileItem tileItem(crossHairDrawPosition, crossHairTexture->Resource, FLinearColor::White);
		tileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(tileItem);

	}
}
