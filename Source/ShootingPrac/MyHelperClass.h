// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/SceneComponent.h"
#include "MyHelperClass.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGPRAC_API UMyHelperClass : public UObject
{
	GENERATED_BODY()

public:
	template<class type>
	static type* GetChildComponentByName(USceneComponent *parent, FString childName)
	{
		for (int i = 0; i < parent->GetNumChildrenComponents(); ++i)
		{
			if (parent->GetChildComponent(i)->GetName() == childName)
				return Cast<type>(parent->GetChildComponent(i));
		}
		return NULL;
	}

};
