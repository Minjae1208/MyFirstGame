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
	static CRITICAL_SECTION	csPlayers;		// CharactersInfo 임계영역

	FuncProcess				fnProcess[100];	// 패킷 처리 구조체

	// 회원가입
	static void SignUp(stringstream & RecvStream, stSocketInfo * pSocket);
	// DB에 로그인
	static void Login(stringstream & RecvStream, stSocketInfo * pSocket);
	// 캐릭터 초기 등록
	static void EnrollCharacter(stringstream & RecvStream, stSocketInfo * pSocket);
	// 캐릭터 위치 동기화
	static void SyncCharacters(stringstream & RecvStream, stSocketInfo * pSocket);
	// 캐릭터 로그아웃 처리
	static void LogoutCharacter(stringstream & RecvStream, stSocketInfo * pSocket);
	// 채팅 수신 후 클라이언트들에게 송신
	static void BroadcastChat(stringstream & RecvStream, stSocketInfo * pSocket);

	// 브로드캐스트 함수
	static void Broadcast(stringstream & SendStream);
	// 다른 클라이언트들에게 새 플레이어 입장 정보 보냄
	static void BroadcastNewPlayer(cCharacter & player);
	// 캐릭터 정보를 버퍼에 기록
	static void WriteCharactersInfoToSocket(stSocketInfo * pSocket);

	// 캐릭터 위치 정보
	static void CharacterPosition(stringstream & RecvStream);

	// 몬스터 정보 초기화
	void InitializeMonsterSet();
};
