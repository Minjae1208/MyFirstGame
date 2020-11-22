#pragma once

#include "ClientSocket.h"

class ClientNetwork
{
private:
	ClientNetwork() 
	{
		for (int nType = 0; nType < EServerType::Max; ++nType)
		{
			m_pSocket[nType] = nullptr;
		}
	}

public:
	static void Recv(int PacketType, stringstream& RecvStream);

	// Login Server
	void RecvLoginServer(stringstream& RecvStream);

	// Ingame Server
	void RecvIngameServer(stringstream& RecvStream);
	void RecvMoveInfo(stringstream& RecvStream);
	void RecvChat(stringstream& RecvStream);
	void RecvNewPlayer(stringstream& RecvStream);

public:
	// Login Server
	static void SendLogin(const TCHAR* ID, const TCHAR* PW);

	// Ingame Server
	static void SendIngame(bool CheckLogin, const TCHAR* Nickname);
	static void SendLogout(int nSessionID);
	static void SendEnterZone(int nSessionID, const FVector& vLocation, const FRotator& rRotation);
	static void SendMoveInfo(int nSessionID, const FVector& vLocation, const FRotator& rRotation);
	static void SendChat(const int SessionID, const TCHAR* Msg);

public:
	static APlayerController* GetPlayerController();

	bool OnLoginServer();
	bool OnIngameServer();

	void StartListen(EServerType eServerType);
	void StopListen(EServerType eServerType);

	void OffLoginServer();
	void OffIngameServer();
	void OffAllServer();

	bool IsConnect(EServerType eServerType);
	ClientSocket* GetClientSocket(EServerType eServerType);

protected:
	ClientSocket* m_pSocket[EServerType::Max];

public:
	static ClientNetwork& Get()
	{
		static ClientNetwork This;
		return This;
	}
};