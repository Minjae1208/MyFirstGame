// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NPlayerController_Login.generated.h"

class ClientSocket;
class UUserWidget;
/**
 * 
 */
UCLASS()
class NPROJECT_API ANPlayerController_Login : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	UUserWidget* BP_LoginUI;

public:
	void SendLogin(const FText& strID, const FText& strPW);
	void RecvLogin(bool bLoginResult, FString strNickname);

	bool SendIngame(FString strNickname);
	void RecvIngame(int nSessionID);

protected:
	void CreateUI();

private :
	bool LoginServerConnected;
};
