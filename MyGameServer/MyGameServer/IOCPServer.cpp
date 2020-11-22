#include "stdafx.h"

#pragma comment(lib, "ws2_32.lib")
#include "IOCPServer.h"
#include "IOCPBase.h"
#include <process.h>
#include <sstream>
#include <algorithm>
#include <string>

// static 변수 초기화
map<int, SOCKET>	IOCPServer::SessionSocket;
cCharactersInfo		IOCPServer::CharactersInfo;
DBConnection		IOCPServer::Con;
CRITICAL_SECTION	IOCPServer::csPlayers;
#include <cstdlib>

unsigned int WINAPI CallWorkerThread(LPVOID p)
{	
	IOCPServer* pOverlappedEvent = (IOCPServer*)p;
	pOverlappedEvent->WorkerThread();
	return 0;
}

IOCPServer::IOCPServer()
{
	InitializeCriticalSection(&csPlayers);

	// DB 접속
	if (Con.Connect(DB_ADDRESS, DB_ID, DB_PW, DB_SCHEMA, DB_PORT))
	{
		printf_s("[INFO] DB 접속 성공\n");
	}
	else {
		printf_s("[ERROR] DB 접속 실패\n");
	}

	// 패킷 함수 포인터에 함수 지정
	fnProcess[EPacketType::CS_SIGNUP].funcProcessPacket = SignUp; // 회원가입
	fnProcess[EPacketType::CS_INGAME_SERVER].funcProcessPacket = Login;
	fnProcess[EPacketType::CS_ENTER_ZONE].funcProcessPacket = EnrollCharacter;
	fnProcess[EPacketType::CS_MOVE_INFO].funcProcessPacket = SyncCharacters;
	fnProcess[EPacketType::CS_CHAT].funcProcessPacket = BroadcastChat;
	fnProcess[EPacketType::CS_LOGOUT_PLAYER].funcProcessPacket = LogoutCharacter;
}


IOCPServer::~IOCPServer()
{
	// winsock 의 사용을 끝낸다
	WSACleanup();
	// 다 사용한 객체를 삭제
	if (SocketInfo)
	{
		delete[] SocketInfo;
		SocketInfo = NULL;
	}

	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = NULL;
	}

	// DB 연결 종료
	Con.Close();
}

void IOCPServer::StartServer()
{
	IOCPBase::StartServer();
}

bool IOCPServer::CreateWorkerThread()
{
	unsigned int threadId;
	// 시스템 정보 가져옴
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO] CPU 갯수 : %d\n", sysInfo.dwNumberOfProcessors);
	// 적절한 작업 스레드의 갯수는 (CPU * 2) + 1
	int nThreadCnt = sysInfo.dwNumberOfProcessors * 2;

	// thread handler 선언
	hWorkerHandle = new HANDLE[nThreadCnt];
	// thread 생성
	for (int i = 0; i < nThreadCnt; i++)
	{
		hWorkerHandle[i] = (HANDLE *)_beginthreadex(
			NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &threadId
		);
		if (hWorkerHandle[i] == NULL)
		{
			printf_s("[ERROR] Worker Thread 생성 실패\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);
	}
	printf_s("[INFO] Worker Thread 시작...\n");
	return true;
}

void IOCPServer::Send(stSocketInfo * pSocket)
{
	int nResult;
	DWORD	sendBytes;
	DWORD	dwFlags = 0;

	nResult = WSASend(
		pSocket->socket,
		&(pSocket->dataBuf),
		1,
		&sendBytes,
		dwFlags,
		NULL,
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR] WSASend 실패 : %d", WSAGetLastError());
	}
}


void IOCPServer::WorkerThread()
{
	// 함수 호출 성공 여부
	BOOL	bResult;
	// Overlapped I/O 작업에서 전송된 데이터 크기
	DWORD	recvBytes;
	// Completion Key를 받을 포인터 변수
	stSocketInfo* pCompletionKey;
	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
	stSocketInfo *	pSocketInfo;
	DWORD	dwFlags = 0;
	
	while (bWorkerThread)
	{
		
		
		/**
		 * 이 함수로 인해 쓰레드들은 WaitingThread Queue 에 대기상태로 들어가게 됨
		 * 완료된 Overlapped I/O 작업이 발생하면 IOCP Queue 에서 완료된 작업을 가져와
		 * 뒷처리를 함
		 */
		bResult = GetQueuedCompletionStatus(hIOCP,
			&recvBytes,				// 실제로 전송된 바이트
			(PULONG_PTR)&pCompletionKey,	// completion key
			(LPOVERLAPPED *)&pSocketInfo,			// overlapped I/O 객체
			INFINITE				// 대기할 시간
		);
		if (bResult == false || recvBytes == 0)
		{
			cout << "[INFO] Socket(" << pSocketInfo->socket << ") 접속 끊김\n" << endl;
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}
		pSocketInfo->dataBuf.len = recvBytes;

		if (bResult == false && recvBytes == 0 && pSocketInfo == NULL)
		{
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}
		
		

		/*try
		{*/
			// 패킷 종류
			int PacketType;
			// 클라이언트 정보 역직렬화
			stringstream RecvStream;
			
			RecvStream << pSocketInfo->dataBuf.buf;
			RecvStream >> PacketType;
			// 패킷 처리
			
			if (fnProcess[PacketType].funcProcessPacket != nullptr)
			{
				fnProcess[PacketType].funcProcessPacket(RecvStream, pSocketInfo);
			}
			else
			{
				printf_s("[ERROR] 정의 되지 않은 패킷 : %d\n", PacketType);
			}
		/*}
		catch (const std::exception& e)
		{
			printf_s("[ERROR] 알 수 없는 예외 발생 : %s\n", e.what());
		}*/
		// 클라이언트 대기
		Recv(pSocketInfo);
	}
}

void IOCPServer::SignUp(stringstream & RecvStream, stSocketInfo * pSocket)
{
	string Id;
	string Pw;

	RecvStream >> Id;
	RecvStream >> Pw;
	cout << "[INFO] 회원가입 시도 {" << Id << "},{" << Pw << "}\n";

	stringstream SendStream;
	SendStream << EPacketType::SC_SIGNUP << endl;
	SendStream << Con.SignUpAccount(Id, Pw) << endl;

	CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->dataBuf.len = SendStream.str().length();

	Send(pSocket);
}

void IOCPServer::Login(stringstream & RecvStream, stSocketInfo * pSocket)
{
	static int s_nSessionID = 0;
	bool CheckLogin;
	bool SendCheck = false;
	string UserCName;

	RecvStream >> CheckLogin;
	RecvStream >> UserCName;
	if (CheckLogin)
	{
		cout << "[INFO] " << UserCName << "님이 인게임으로 접속했습니다." << endl;
		SendCheck = true;
	}

	stringstream SendStream;
	SendStream << EPacketType::SC_INGAME_SERVER << endl;
	SendStream << SendCheck << endl; // true
	SendStream << ++s_nSessionID << endl; // true
	
	CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->dataBuf.len = SendStream.str().length();

	Send(pSocket);
}

void IOCPServer::EnrollCharacter(stringstream & RecvStream, stSocketInfo * pSocket)
{
	cCharacter info;
	RecvStream >> info;
	printf_s("[INFO][%d]캐릭터 등록 - X : [%f], Y : [%f], Z : [%f], Yaw : [%f]\n",
		info.SessionId, info.X, info.Y, info.Z, info.Yaw);

	EnterCriticalSection(&csPlayers);

	cCharacter* pinfo = &CharactersInfo.players[info.SessionId];

	// 캐릭터의 위치를 저장						
	pinfo->SessionId = info.SessionId;
	pinfo->X = info.X;
	pinfo->Y = info.Y;
	pinfo->Z = info.Z;

	// 캐릭터의 회전값을 저장
	pinfo->Yaw = info.Yaw;
	pinfo->Pitch = info.Pitch;
	pinfo->Roll = info.Roll;

	// 캐릭터의 속도를 저장
	pinfo->VX = info.VX;
	pinfo->VY = info.VY;
	pinfo->VZ = info.VZ;

	LeaveCriticalSection(&csPlayers);

	SessionSocket[info.SessionId] = pSocket->socket;

	cout << "[INFO] 클라이언트 수 : " << SessionSocket.size() << endl;

	//Send(pSocket);
	BroadcastNewPlayer(info);
}

void IOCPServer::SyncCharacters(stringstream& RecvStream, stSocketInfo* pSocket)
{
	cCharacter info;
	RecvStream >> info;

	// 	 	printf_s("[INFO][%d]정보 수신 - %d\n",
	// 	 		info.SessionId, info.IsAttacking);	
	EnterCriticalSection(&csPlayers);

	cCharacter * pinfo = &CharactersInfo.players[info.SessionId];

	// 캐릭터의 위치를 저장						
	pinfo->SessionId = info.SessionId;
	pinfo->X = info.X;
	pinfo->Y = info.Y;
	pinfo->Z = info.Z;

	// 캐릭터의 회전값을 저장
	pinfo->Yaw = info.Yaw;
	pinfo->Pitch = info.Pitch;
	pinfo->Roll = info.Roll;

	// 캐릭터의 속도를 저장
	pinfo->VX = info.VX;
	pinfo->VY = info.VY;
	pinfo->VZ = info.VZ;

	LeaveCriticalSection(&csPlayers);

	//WriteCharactersInfoToSocket(pSocket);
	//Send(pSocket);
	stringstream SendStream;
	SendStream << EPacketType::SC_MOVE_INFO << endl;
	SendStream << CharactersInfo << endl;

	Broadcast(SendStream);
}

void IOCPServer::LogoutCharacter(stringstream& RecvStream, stSocketInfo* pSocket)
{
	int SessionId;
	RecvStream >> SessionId;
	printf_s("[INFO] (%d)로그아웃 요청 수신\n", SessionId);
	EnterCriticalSection(&csPlayers);

	CharactersInfo.players.erase(SessionId);

	LeaveCriticalSection(&csPlayers);
	SessionSocket.erase(SessionId);
	cout << "[INFO] 클라이언트 수 : " << SessionSocket.size() << endl;
	//WriteCharactersInfoToSocket(pSocket);
}


void IOCPServer::BroadcastChat(stringstream& RecvStream, stSocketInfo* pSocket)
{
	setlocale(LC_ALL, "ko_KR.utf8");

	stSocketInfo* client = new stSocketInfo;

	int SessionId;
	string Temp;
	string Chat;
	RecvStream >> SessionId;
	getline(RecvStream, Temp);
	Chat += to_string(SessionId) + "_:_";
	while (RecvStream >> Temp)
	{
		Chat += Temp + "_";
	}
	Chat += '\0';
	cout << "[CHAT] " << Chat.c_str() << endl;

	stringstream SendStream;
	SendStream << EPacketType::SC_CHAT << endl;
	SendStream << Chat;

	Broadcast(SendStream);
}

void IOCPServer::BroadcastNewPlayer(cCharacter & player)
{
	stringstream SendStream;
	SendStream << EPacketType::SC_ENTER_NEW_PLAYER << endl;
	//SendStream << player << endl;
	SendStream << CharactersInfo << endl;

	Broadcast(SendStream);
}

void IOCPServer::Broadcast(stringstream & SendStream)
{
	stSocketInfo* client = new stSocketInfo;
	for (const auto& kvp : SessionSocket)
	{
		client->socket = kvp.second;
		CopyMemory(client->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
		client->dataBuf.buf = client->messageBuffer;
		client->dataBuf.len = SendStream.str().length();

		Send(client);
	}
}

void IOCPServer::WriteCharactersInfoToSocket(stSocketInfo * pSocket)
{
	// 현재는 사용하지 않아서 주석 처리
	//stringstream SendStream;

	//// 직렬화	
	//SendStream << EPacketType::RECV_PLAYER << endl;
	//SendStream << CharactersInfo << endl;

	//// !!! 중요 !!! data.buf 에다 직접 데이터를 쓰면 쓰레기값이 전달될 수 있음
	//CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
	//pSocket->dataBuf.buf = pSocket->messageBuffer;
	//pSocket->dataBuf.len = SendStream.str().length();
}

//void IOCPServer::CharacterPosition(stringstream & RecvStream, stSocketInfo * pSocket)
//{
//
//}
