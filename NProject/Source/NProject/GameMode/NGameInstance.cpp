// Fill out your copyright notice in the Description page of Project Settings.


#include "NGameInstance.h"

#include "Network/ClientNetwork.h"

void UNGameInstance::Init()
{
	Super::Init();
}

void UNGameInstance::Shutdown()
{
	Super::Shutdown();

	ClientNetwork::Get().OffAllServer();
}