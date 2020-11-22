// Fill out your copyright notice in the Description page of Project Settings.


#include "NPlayerController_Login.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

#include "Network/ClientNetwork.h"
#include "Manager/CharacterManager.h"

#define LOGIN_SERVER_PORT 8000
#define INGAME_SERVER_PORT 4000

void ANPlayerController_Login::BeginPlay()
{
	Super::BeginPlay();

	CreateUI();

	// 로그인 서버 접속
	if (ClientNetwork::Get().OnLoginServer() == false)
	{
		UE_LOG(LogClass, Log, TEXT("Failed : Login Server connect!"));
	}

	bShowMouseCursor = true;
}

void ANPlayerController_Login::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void ANPlayerController_Login::CreateUI()
{
	UWorld* pWorld = GetWorld();
	if (pWorld == nullptr)
		return;

	FSoftClassPath refClass(TEXT("/Game/UI/BP_Login.BP_Login_C"));
	UClass* pClass = refClass.ResolveClass();
	if (pClass == nullptr)
		pClass = refClass.TryLoadClass<UUserWidget>();

	BP_LoginUI = Cast<UUserWidget>(CreateWidget<UUserWidget>(pWorld, pClass));
	if (BP_LoginUI == nullptr)
		return;

	// 화면에 붙이기
	BP_LoginUI->AddToViewport(0);
}

void ANPlayerController_Login::SendLogin(const FText& strID, const FText& strPW)
{
	if (strID.IsEmpty() || strPW.IsEmpty())
		return;

	if (ClientNetwork::Get().IsConnect(EServerType::LOGIN_SERVER) == false)
		return;

	ClientNetwork::Get().StartListen(EServerType::LOGIN_SERVER);

	ClientNetwork::Get().SendLogin(*strID.ToString(), *strPW.ToString());
}

void ANPlayerController_Login::RecvLogin(bool bLoginResult, FString strNickname)
{
	if (bLoginResult == false)
	{
		UE_LOG(LogClass, Log, TEXT("Failed : InGame Server Login "));
		return;
	}

	CharacterManager::Get().SetMyNickname(strNickname);

	SendIngame(strNickname);

	// 로그인 서버 소켓 종료
	ClientNetwork::Get().OffLoginServer();
}

bool ANPlayerController_Login::SendIngame(FString strNickname)
{
	// 인게임 서버 접속
	if (ClientNetwork::Get().OnIngameServer() == false)
		return false;

	ClientNetwork::Get().StartListen(EServerType::INGAME_SERVER);

	// 인게임 서버 로그인
	ClientNetwork::Get().SendIngame(true, *strNickname);

	return true;
}

void ANPlayerController_Login::RecvIngame(int nSessionID)
{
	CharacterManager::Get().SetMySessionID(nSessionID);

	UWorld* pWorld = GetWorld();
	if (pWorld == nullptr)
		return;

	UGameplayStatics::OpenLevel(pWorld, TEXT("Ingame"));
}