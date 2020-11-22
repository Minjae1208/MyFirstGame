// Fill out your copyright notice in the Description page of Project Settings.


#include "NGameMode_Login.h"
#include "PlayerController/NPlayerController_Login.h"

#include "Network/ClientSocket.h"

ANGameMode_Login::ANGameMode_Login()
{
	DefaultPawnClass = nullptr;
	GameStateClass = nullptr;
	PlayerControllerClass = ANPlayerController_Login::StaticClass();
	//PlayerStateClass = nullptr;
}

void ANGameMode_Login::BeginPlay()
{
	Super::BeginPlay();
}

void ANGameMode_Login::StartPlay()
{
	Super::StartPlay();
}