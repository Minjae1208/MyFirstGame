#include "IOCPBase.h"
#include <process.h>
#include <sstream>
#include <algorithm>
#include <string>

IOCPBase::IOCPBase()
{
	bWorkerThread = true; // Server WorkerThread flag true = start, false = stop
	bAccept = true; // Wait Client Socket
}

IOCPBase::~IOCPBase()
{
	// winsock�� ����� ����
	WSACleanup();

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
}


bool IOCPBase::Init()
{
	WSADATA wsaData;
	int nResult;

	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (nResult != 0)
	{
		printf_s("[ERROR][LoginServer] Winsock �ʱ�ȭ ���� \n");
		return false;
	}

	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (ListenSocket == INVALID_SOCKET)
	{
		printf("[ERROR][LoginServer] ���� ���� ����\n");
		return false;
	}

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(SERVER_PORT);

	if (::bind(ListenSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf_s("[ERROR][LoginServer] bind ���� \n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	nResult = listen(ListenSocket, 5);
	if (nResult == SOCKET_ERROR)
	{
		printf_s("[ERROR][LoginServer] listen ����\n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	return true;
}


void IOCPBase::StartLoginServer()
{
	int nResult;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKET clientSocket;
	DWORD recvBytes;
	DWORD flags;

	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (!CreateWorkerThread()) return;

	printf("[INFO][LoginServer] ���� ����...\n");

	while (bAccept)
	{
		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)&clientAddr, &addrLen, NULL, NULL);

		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR][LoginServer] Accept ���� \n");
			return;
		}

		SocketInfo = new stSocketInfo();
		SocketInfo->socket = clientSocket;
		SocketInfo->recvBytes = 0;
		SocketInfo->sendBytes = 0;
		SocketInfo->dataBuf.len = MAX_BUFFER;
		SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
		flags = 0;

		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (DWORD)SocketInfo, 0);

		nResult = WSARecv(SocketInfo->socket, &SocketInfo->dataBuf, 1, &recvBytes, &flags, &(SocketInfo->overlapped), NULL);



		if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR][LoginServer] IO Pending ���� : %d", WSAGetLastError());
			return;
		}
	}
}

bool IOCPBase::CreateWorkerThread()
{
	return false; // !CreateWorkerThread() << !false -> true
}


void IOCPBase::Send(stSocketInfo* pSocket)
{
	int nResult;
	DWORD sendBytes;
	DWORD dwFlags = 0;

	nResult = WSASend(pSocket->socket, &(pSocket->dataBuf), 1, &sendBytes, dwFlags, NULL, NULL);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR][LoginServer] WSASend ���� : %d", WSAGetLastError());
	}
}


void IOCPBase::Recv(stSocketInfo* pSocket)
{
	int nResult;

	DWORD dwFlags = 0;

	ZeroMemory(&(pSocket->overlapped), sizeof(OVERLAPPED));
	ZeroMemory(pSocket->messageBuffer, MAX_BUFFER);

	pSocket->dataBuf.len = MAX_BUFFER;
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->recvBytes = 0;
	pSocket->sendBytes = 0;

	dwFlags = 0;
	nResult = WSARecv(pSocket->socket, &(pSocket->dataBuf), 1, (LPDWORD)&pSocket, &dwFlags, (LPWSAOVERLAPPED)&(pSocket->overlapped), NULL);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR][LoginServer] WSARecv ���� : %d", WSAGetLastError());
	}
}

void IOCPBase::WorkerThread()
{

}