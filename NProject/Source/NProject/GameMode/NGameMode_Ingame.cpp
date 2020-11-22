// Copyright Epic Games, Inc. All Rights Reserved.

#include "NGameMode_Ingame.h"
#include "UObject/ConstructorHelpers.h"

#include "Actor/NCharacter.h"
#include "PlayerController/NPlayerController_Ingame.h"

ANGameMode_Ingame::ANGameMode_Ingame()
{
	// set default pawn class to our Blueprinted character
	
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Chracter/BP_NCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = ANPlayerController_Ingame::StaticClass();
}

void ANGameMode_Ingame::BeginPlay()
{
	Super::BeginPlay();
}

void ANGameMode_Ingame::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}