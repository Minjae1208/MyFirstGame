
#include "ClientNetwork.h"
#include "Kismet/GameplayStatics.h"

#include "Packet.h"
#include "Manager/CharacterManager.h"
#include "Actor/NCharacter.h"
#include "PlayerController/NPlayerController_Login.h"
#include "PlayerController/NPlayerController_Ingame.h"

#define LOGIN_SERVER_PORT 8000
#define INGAME_SERVER_PORT 4000

void ClientNetwork::Recv(int PacketType, stringstream& RecvStream)
{
	ClientNetwork& kClientNetwork = ClientNetwork::Get();

	switch (PacketType)
	{
		// Login Server
	case EPacketType::SC_LOGIN_SERVER:		kClientNetwork.RecvLoginServer(RecvStream); break;

		// Ingame Server
	case EPacketType::SC_INGAME_SERVER:		kClientNetwork.RecvIngameServer(RecvStream); break;

	case EPacketType::SC_CHAT:				kClientNetwork.RecvChat(RecvStream); break;

	case EPacketType::SC_MOVE_INFO:			kClientNetwork.RecvMoveInfo(RecvStream); break;
	case EPacketType::SC_ENTER_NEW_PLAYER:	kClientNetwork.RecvNewPlayer(RecvStream); break;
	default:
		{
			UE_LOG(LogClass, Log, TEXT("default!"));
			break;
		}
	}
}

void ClientNetwork::RecvLoginServer(stringstream& RecvStream)
{
	RecvPacket_Login recvLogin;
	RecvStream >> recvLogin;

	ANPlayerController_Login* pPlayerController = Cast< ANPlayerController_Login>(GetPlayerController());
	if (pPlayerController == nullptr ||
		pPlayerController->IsPendingKill())
		return;

	wstring wstrMsg;
	wstrMsg.assign(recvLogin.UserCName.begin(), recvLogin.UserCName.end());

	pPlayerController->RecvLogin(recvLogin.LoginResult, wstrMsg.c_str());
}

void ClientNetwork::RecvIngameServer(stringstream& RecvStream)
{
	ANPlayerController_Login* pPlayerController = Cast< ANPlayerController_Login>(GetPlayerController());
	if (pPlayerController == nullptr ||
		pPlayerController->IsPendingKill())
		return;

	RecvPacket_Ingame recvIngame;
	RecvStream >> recvIngame;

	pPlayerController->RecvIngame(recvIngame.SessionID);
}

void ClientNetwork::RecvMoveInfo(stringstream& RecvStream)
{
	PacketCharactersInfo m_kCharactersInfo;
	RecvStream >> m_kCharactersInfo;
	
	for (auto itCharacter : m_kCharactersInfo.players)
	{
		// 내정보는 무시한다.
		if (CharacterManager::Get().IsMyCharacter(itCharacter.second.SessionID))
			continue;
		
		ANCharacter* pCharacter = CharacterManager::Get().GetCharacter(itCharacter.second.SessionID);
		if (pCharacter == nullptr)
			continue;

		FVector vLocation(itCharacter.second.X, itCharacter.second.Y, itCharacter.second.Z);
		FRotator rRotation(itCharacter.second.Pitch, itCharacter.second.Yaw, itCharacter.second.Roll);

		pCharacter->SetActorLocationAndRotation(vLocation, rRotation);
	}
}

void ClientNetwork::RecvChat(stringstream& RecvStream)
{
	string sChat;
	RecvStream >> sChat;

	string d;
	ANPlayerController_Ingame* pMyController = Cast<ANPlayerController_Ingame>(GetPlayerController());
	if (pMyController == nullptr)
		return;

	string strMsg(sChat);
	wstring wstrMsg;
	wstrMsg.assign(strMsg.begin(), strMsg.end());

	pMyController->AddChat(wstrMsg.c_str());
}

void ClientNetwork::RecvNewPlayer(stringstream& RecvStream)
{
	PacketCharactersInfo m_kCharactersInfo;
	RecvStream >> m_kCharactersInfo;

	for (auto itCharacter : m_kCharactersInfo.players)
	{
		if (CharacterManager::Get().IsMyCharacter(itCharacter.second.SessionID) == true)
			continue;

		if (CharacterManager::Get().HasCharacter(itCharacter.second.SessionID) == true)
			continue;

		ANPlayerController_Ingame* pMyController = Cast<ANPlayerController_Ingame>(GetPlayerController());
		if (pMyController == nullptr)
			continue;

		pMyController->CreateCharacter(itCharacter.second.SessionID);
	}
}

void ClientNetwork::SendLogin(const TCHAR* ID, const TCHAR* PW)
{
	ClientSocket* pClientSocket = ClientNetwork::Get().GetClientSocket(EServerType::LOGIN_SERVER);
	if (pClientSocket == nullptr)
		return;

	stringstream SendStream;

	wstring wstrID(ID), wstrPW(PW);
	string strID, strPW;
	strID.assign(wstrID.begin(), wstrID.end());
	strPW.assign(wstrPW.begin(), wstrPW.end());

	SendStream << EPacketType::CS_LOGIN_SERVER << endl;
	SendStream << strID << endl;
	SendStream << strPW << endl;

	pClientSocket->SendPacket(SendStream);
}

void ClientNetwork::SendIngame(bool CheckLogin, const TCHAR* Nickname)
{
	ClientSocket* pClientSocket = ClientNetwork::Get().GetClientSocket(EServerType::INGAME_SERVER);
	if (pClientSocket == nullptr)
		return;

	wstring wstrName(Nickname);
	string strName;
	strName.assign(wstrName.begin(), wstrName.end());

	stringstream SendStream;

	SendStream << (int)EPacketType::CS_INGAME_SERVER << endl;
	SendStream << CheckLogin << endl;
	SendStream << strName << endl;

	pClientSocket->SendPacket(SendStream);
}

void ClientNetwork::SendLogout(int nSessionID)
{
	ClientSocket* pClientSocket = ClientNetwork::Get().GetClientSocket(EServerType::INGAME_SERVER);
	if (pClientSocket == nullptr)
		return;

	stringstream SendStream;
	SendStream << EPacketType::CS_LOGOUT_PLAYER << endl;
	SendStream << nSessionID << endl;

	pClientSocket->SendPacket(SendStream);
}

void ClientNetwork::SendEnterZone(int nSessionID, const FVector& vLocation, const FRotator& rRotation)
{
	ClientSocket* pClientSocket = ClientNetwork::Get().GetClientSocket(EServerType::INGAME_SERVER);
	if (pClientSocket == nullptr)
		return;

	stringstream SendStream;

	Packet_MoveInfo info;
	info.SessionID = nSessionID;
	info.X = vLocation.X;
	info.Y = vLocation.Y;
	info.Z = vLocation.Z;

	info.Yaw = rRotation.Yaw;
	info.Pitch = rRotation.Pitch;
	info.Roll = rRotation.Roll;

	SendStream << EPacketType::CS_ENTER_ZONE << endl;
	SendStream << info;

	pClientSocket->SendPacket(SendStream);
}

void ClientNetwork::SendMoveInfo(int nSessionID, const FVector& vLocation, const FRotator& rRotation)
{
	ClientSocket* pClientSocket = ClientNetwork::Get().GetClientSocket(EServerType::INGAME_SERVER);
	if (pClientSocket == nullptr)
		return;

	Packet_MoveInfo info;
	info.SessionID = nSessionID;
	info.X = vLocation.X;
	info.Y = vLocation.Y;
	info.Z = vLocation.Z;

	info.Yaw = rRotation.Yaw;
	info.Pitch = rRotation.Pitch;
	info.Roll = rRotation.Roll;

	stringstream SendStream;
	SendStream << EPacketType::CS_MOVE_INFO << endl;
	SendStream << info;

	pClientSocket->SendPacket(SendStream);
}

void ClientNetwork::SendChat(const int SessionID, const TCHAR* Msg)
{
	ClientSocket* pClientSocket = ClientNetwork::Get().GetClientSocket(EServerType::INGAME_SERVER);
	if (pClientSocket == nullptr)
		return;

	wstring wstrMsg(Msg);
	string  Message;
	Message.assign(wstrMsg.begin(), wstrMsg.end());

	stringstream SendStream;
	SendStream << EPacketType::CS_CHAT << endl;
	SendStream << SessionID << endl;
	SendStream << Message << endl;

	pClientSocket->SendPacket(SendStream);
}

UWorld* GetWorld()
{
	const FWorldContext* WorldContext = nullptr;
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		WorldContext = &Context;
		if (WorldContext->OwningGameInstance != nullptr)
		{
			UWorld* pWorld = WorldContext->World();
			if (pWorld && pWorld->bIsTearingDown == false)
			{
				return pWorld;
			}
		}
	}

	return nullptr;
}

APlayerController* ClientNetwork::GetPlayerController()
{
	UWorld* pWorld = GetWorld();
	if (pWorld == nullptr)
		return nullptr;

	return UGameplayStatics::GetPlayerController(pWorld, 0);
}

bool ClientNetwork::OnLoginServer()
{
	ClientSocket* pClientSocket = new ClientSocket();
	if (pClientSocket == nullptr)
		return false;

	if (pClientSocket->InitSocket() == false)
	{
		delete pClientSocket;
		return false;
	}

	if (pClientSocket->Connect("127.0.0.1", LOGIN_SERVER_PORT) == false)
	{
		delete pClientSocket;
		return false;
	}

	UE_LOG(LogClass, Log, TEXT("Login Server connect success!"));
	m_pSocket[EServerType::LOGIN_SERVER] = pClientSocket;

	return true;
}

bool ClientNetwork::OnIngameServer()
{
	ClientSocket* pClientSocket = new ClientSocket();
	if (pClientSocket == nullptr)
		return false;

	if (pClientSocket->InitSocket() == false)
	{
		delete pClientSocket;
		return false;
	}

	if (pClientSocket->Connect("127.0.0.1", INGAME_SERVER_PORT) == false)
	{
		delete pClientSocket;
		return false;
	}

	UE_LOG(LogClass, Log, TEXT("Success : Ingame Server connect !"));
	m_pSocket[EServerType::INGAME_SERVER] = pClientSocket;

	return true;
}

void ClientNetwork::StartListen(EServerType eServerType)
{
	ClientSocket* pClinetSocket = GetClientSocket(eServerType);
	if (pClinetSocket == nullptr)
		return;

	pClinetSocket->StartListen();
}

void ClientNetwork::StopListen(EServerType eServerType)
{
	ClientSocket* pClinetSocket = GetClientSocket(eServerType);
	if (pClinetSocket == nullptr)
		return;

	pClinetSocket->StopListen();
}

void ClientNetwork::OffLoginServer()
{
	if (m_pSocket[EServerType::LOGIN_SERVER] == nullptr)
		return;

	m_pSocket[EServerType::LOGIN_SERVER]->CloseSocket();
	m_pSocket[EServerType::LOGIN_SERVER]->StopListen();

	delete m_pSocket[EServerType::LOGIN_SERVER];
	m_pSocket[EServerType::LOGIN_SERVER] = nullptr;
}

void ClientNetwork::OffIngameServer()
{
	if (m_pSocket[EServerType::INGAME_SERVER] == nullptr)
		return;

	m_pSocket[EServerType::INGAME_SERVER]->CloseSocket();
	m_pSocket[EServerType::INGAME_SERVER]->StopListen();

	delete m_pSocket[EServerType::INGAME_SERVER];
	m_pSocket[EServerType::INGAME_SERVER] = nullptr;
}

void ClientNetwork::OffAllServer()
{
	for (int nType = 0; nType < EServerType::Max; ++nType)
	{
		if (m_pSocket[nType] == nullptr)
			continue;

		m_pSocket[nType]->CloseSocket();
		m_pSocket[nType]->StopListen();

		m_pSocket[nType] = nullptr;
	}
}

bool ClientNetwork::IsConnect(EServerType eServerType)
{
	if (eServerType < 0 ||
		eServerType >= EServerType::Max)
		return false;

	return m_pSocket[eServerType] != nullptr;
}

ClientSocket* ClientNetwork::GetClientSocket(EServerType eServerType)
{
	if (eServerType < 0 ||
		eServerType >= EServerType::Max)
		return nullptr;

	return m_pSocket[eServerType];
}
