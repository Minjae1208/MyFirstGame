#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WInSock2.h>
#include <map>
#include <vector>
#include <iostream>

using namespace std;

#define MAX_BUFFER 4096
#define SERVER_PORT 8000
#define MAX_CLIENTS 20


struct stSocketInfo
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuf;
	SOCKET socket;
	char messageBuffer[MAX_BUFFER];
	int recvBytes;
	int sendBytes;
};

// 
struct FuncProcess
{
	void(*funcProcessPacket)(stringstream &RecvStream, stSocketInfo* pSocket);
	// SignUp(매개변수1, 매개변수2)
	FuncProcess()
	{
		funcProcessPacket = nullptr;
	}
};


class IOCPBase
{
public:
	IOCPBase();
	virtual ~IOCPBase();

	// 소켓 등록 및 서버 정보 설정
	bool Init();

	// 서버 start
	virtual void StartLoginServer();

	// 작업 스레드 생성
	virtual bool CreateWorkerThread();

	// 작업 스레드
	virtual void WorkerThread();

	//클라이언트로 송신 
	virtual void Send(stSocketInfo* pSocket);

	//클라이언트 수신 대기
	virtual void Recv(stSocketInfo* pSocket);

	stSocketInfo * SocketInfo;	// 소켓정보
	SOCKET ListenSocket;		// 서버 리슨 소켓
	HANDLE hIOCP;				// IOCP 객체 핸들
	bool bAccept;				// 요청 동작 플래그
	bool bWorkerThread;			// 작업 스레드 동작 플래그
	HANDLE* hWorkerHandle;		// 작업 스레드 핸들
	int nThreadOut;
};