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
	// SignUp(�Ű�����1, �Ű�����2)
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

	// ���� ��� �� ���� ���� ����
	bool Init();

	// ���� start
	virtual void StartLoginServer();

	// �۾� ������ ����
	virtual bool CreateWorkerThread();

	// �۾� ������
	virtual void WorkerThread();

	//Ŭ���̾�Ʈ�� �۽� 
	virtual void Send(stSocketInfo* pSocket);

	//Ŭ���̾�Ʈ ���� ���
	virtual void Recv(stSocketInfo* pSocket);

	stSocketInfo * SocketInfo;	// ��������
	SOCKET ListenSocket;		// ���� ���� ����
	HANDLE hIOCP;				// IOCP ��ü �ڵ�
	bool bAccept;				// ��û ���� �÷���
	bool bWorkerThread;			// �۾� ������ ���� �÷���
	HANDLE* hWorkerHandle;		// �۾� ������ �ڵ�
	int nThreadOut;
};