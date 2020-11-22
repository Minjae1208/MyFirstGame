// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NGameMode_Ingame.generated.h"

UCLASS(minimalapi)
class ANGameMode_Ingame : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANGameMode_Ingame();

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	//~ End AActor Interface
};



