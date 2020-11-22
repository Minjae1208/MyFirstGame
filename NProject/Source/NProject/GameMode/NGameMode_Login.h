// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NGameMode_Login.generated.h"

class ClientSocket;
/**
 * 
 */
UCLASS()
class NPROJECT_API ANGameMode_Login : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ANGameMode_Login();

	virtual void BeginPlay() override;

	virtual void StartPlay() override;

protected:
	/*ClientSocket* Socket;*/

};
