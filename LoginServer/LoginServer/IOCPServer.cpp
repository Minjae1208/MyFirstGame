#include "stdafx.h"

#pragma comment(lib, "ws2_32.lib")
#include "IOCPServer.h"
#include "IOCPBase.h"
#include <process.h>
#include <sstream>
#include <algorithm>
#include <string>
#include <locale.h>

// static 변수 초기화
map<int, SOCKET>	IOCPServer::SessionSocket;
DBConnect		IOCPServer::Con;

unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	IOCPServer* pOverlappedEvent = (IOCPServer*)p;
	pOverlappedEvent->WorkerThread();
	return 0;
}

IOCPServer::IOCPServer()
{
	// DB 접속
	if (Con.Connect(DB_ADDRESS, DB_ID, DB_PW, DB_SCHEMA, DB_PORT))
	{
		printf_s("[INFO][LoginServer] DB 접속 성공\n");
	}
	else {
		printf_s("[ERROR][LoginServer] DB 접속 실패\n");
	}

	// 패킷 함수 포인터에 함수 지정
	fnProcess[EPacketType::CS_SIGNUP].funcProcessPacket = SignUp; // 회원가입
	fnProcess[EPacketType::CS_LOGIN_SERVER].funcProcessPacket = Login;
	//fnProcess[EPacketType::EXIT].funcProcessPacket = Exit;
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

void IOCPServer::StartLoginServer()
{
	IOCPBase::StartLoginServer();
}

bool IOCPServer::CreateWorkerThread()
{
	unsigned int threadId;
	// 시스템 정보 가져옴
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO][LoginServer] CPU 갯수 : %d\n", sysInfo.dwNumberOfProcessors);
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
			printf_s("[ERROR][LoginServer] Worker Thread 생성 실패\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);
	}
	printf_s("[INFO][LoginServer] Worker Thread 시작...\n");
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
		printf_s("[ERROR][LoginServer] WSASend 실패 : %d", WSAGetLastError());
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
			cout << "[INFO][LoginServer] Socket(" << pSocketInfo->socket << ") 접속 끊김\n" << endl;
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



		try
		{
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
			printf_s("[ERROR][LoginServer] 정의 되지 않은 패킷 : %d\n", PacketType);
		}
		}
		catch (const std::exception& e)
		{
			printf_s("[ERROR][LoginServer] 알 수 없는 예외 발생 : %s\n", e.what());
		}
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
	cout << "[INFO][LoginServer] 회원가입 시도 {" << Id << "},{" << Pw << "}\n";

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
	string Id;
	string Pw;
	string UserCName;
	bool CheckLogin;
	RecvStream >> Id;
	RecvStream >> Pw;
	cout << "[INFO][LoginServer] 로그인 시도 {" << Id << "},{" << Pw << "}" << endl;

	#if 1
	CheckLogin = true;
	UserCName = "test\0";
	cout << "[INFO][LoginServer] " << UserCName << " 로그인 성공!" << endl;
#else
	CheckLogin = Con.SearchAccount(Id, Pw);

	if (CheckLogin)
	{
		UserCName = Con.GetCName(Id);
		UserCName += "\0";
		cout << "[INFO][LoginServer] " << UserCName << " 로그인 성공!" << endl;
	}
	else
		cout << "[INFO][LoginServer] 로그인 실패!" << endl;
	#endif

	

	stringstream SendStream;
	SendStream << EPacketType::SC_LOGIN_SERVER << endl;
	SendStream << CheckLogin << endl; // true
	SendStream << UserCName << endl;

	CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->dataBuf.len = SendStream.str().length();

	Send(pSocket);
}

void IOCPServer::Exit(stringstream & RecvStream, stSocketInfo * pSocket)
{
	closesocket(pSocket->socket);
	free(pSocket);
}
