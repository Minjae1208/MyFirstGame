#pragma once


#define _WINSOCK_DEPREACTED_NO_WARNINGS

#include <WinSock2.h>
#include <map>
#include <vector>
#include <iostream>
#include "CommonClass.h"
#include "DBConnection.h"
#include "IOCPBase.h"

using namespace std;

#define DB_ADDRESS "localhost"
#define DB_PORT 3306
#define DB_ID "root"
#define DB_PW "chaminjae123@"
#define DB_SCHEMA "3Dgameuser"

class IOCPServer: public IOCPBase
{
public :
	IOCPServer();
	virtual ~IOCPServer();

	virtual void StartServer() override;

	virtual bool CreateWorkerThread() override;

	virtual void WorkerThread() override;

	static void Send(stSocketInfo* pSockert);

private :
	static cCharactersInfo CharactersInfo;
	static map<int, SOCKET> SessionSocket;
	static DBConnection Con;
	static CRITICAL_SECTION	csPlayers;		// CharactersInfo �Ӱ迵��

	FuncProcess				fnProcess[100];	// ��Ŷ ó�� ����ü

	// ȸ������
	static void SignUp(stringstream & RecvStream, stSocketInfo * pSocket);
	// DB�� �α���
	static void Login(stringstream & RecvStream, stSocketInfo * pSocket);
	// ĳ���� �ʱ� ���
	static void EnrollCharacter(stringstream & RecvStream, stSocketInfo * pSocket);
	// ĳ���� ��ġ ����ȭ
	static void SyncCharacters(stringstream & RecvStream, stSocketInfo * pSocket);
	// ĳ���� �α׾ƿ� ó��
	static void LogoutCharacter(stringstream & RecvStream, stSocketInfo * pSocket);
	// ä�� ���� �� Ŭ���̾�Ʈ�鿡�� �۽�
	static void BroadcastChat(stringstream & RecvStream, stSocketInfo * pSocket);

	// ��ε�ĳ��Ʈ �Լ�
	static void Broadcast(stringstream & SendStream);
	// �ٸ� Ŭ���̾�Ʈ�鿡�� �� �÷��̾� ���� ���� ����
	static void BroadcastNewPlayer(cCharacter & player);
	// ĳ���� ������ ���ۿ� ���
	static void WriteCharactersInfoToSocket(stSocketInfo * pSocket);

	// ĳ���� ��ġ ����
	static void CharacterPosition(stringstream & RecvStream);

	// ���� ���� �ʱ�ȭ
	void InitializeMonsterSet();
};
