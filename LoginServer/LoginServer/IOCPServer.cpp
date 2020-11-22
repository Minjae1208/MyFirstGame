#include "stdafx.h"

#pragma comment(lib, "ws2_32.lib")
#include "IOCPServer.h"
#include "IOCPBase.h"
#include <process.h>
#include <sstream>
#include <algorithm>
#include <string>
#include <locale.h>

// static ���� �ʱ�ȭ
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
	// DB ����
	if (Con.Connect(DB_ADDRESS, DB_ID, DB_PW, DB_SCHEMA, DB_PORT))
	{
		printf_s("[INFO][LoginServer] DB ���� ����\n");
	}
	else {
		printf_s("[ERROR][LoginServer] DB ���� ����\n");
	}

	// ��Ŷ �Լ� �����Ϳ� �Լ� ����
	fnProcess[EPacketType::CS_SIGNUP].funcProcessPacket = SignUp; // ȸ������
	fnProcess[EPacketType::CS_LOGIN_SERVER].funcProcessPacket = Login;
	//fnProcess[EPacketType::EXIT].funcProcessPacket = Exit;
}


IOCPServer::~IOCPServer()
{
	// winsock �� ����� ������
	WSACleanup();
	// �� ����� ��ü�� ����
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

	// DB ���� ����
	Con.Close();
}

void IOCPServer::StartLoginServer()
{
	IOCPBase::StartLoginServer();
}

bool IOCPServer::CreateWorkerThread()
{
	unsigned int threadId;
	// �ý��� ���� ������
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO][LoginServer] CPU ���� : %d\n", sysInfo.dwNumberOfProcessors);
	// ������ �۾� �������� ������ (CPU * 2) + 1
	int nThreadCnt = sysInfo.dwNumberOfProcessors * 2;

	// thread handler ����
	hWorkerHandle = new HANDLE[nThreadCnt];
	// thread ����
	for (int i = 0; i < nThreadCnt; i++)
	{
		hWorkerHandle[i] = (HANDLE *)_beginthreadex(
			NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &threadId
		);
		if (hWorkerHandle[i] == NULL)
		{
			printf_s("[ERROR][LoginServer] Worker Thread ���� ����\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);
	}
	printf_s("[INFO][LoginServer] Worker Thread ����...\n");
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
		printf_s("[ERROR][LoginServer] WSASend ���� : %d", WSAGetLastError());
	}
}

void IOCPServer::WorkerThread()
{
	// �Լ� ȣ�� ���� ����
	BOOL	bResult;
	// Overlapped I/O �۾����� ���۵� ������ ũ��
	DWORD	recvBytes;
	// Completion Key�� ���� ������ ����
	stSocketInfo* pCompletionKey;
	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������	
	stSocketInfo *	pSocketInfo;
	DWORD	dwFlags = 0;

	while (bWorkerThread)
	{


		/**
		 * �� �Լ��� ���� ��������� WaitingThread Queue �� �����·� ���� ��
		 * �Ϸ�� Overlapped I/O �۾��� �߻��ϸ� IOCP Queue ���� �Ϸ�� �۾��� ������
		 * ��ó���� ��
		 */
		bResult = GetQueuedCompletionStatus(hIOCP,
			&recvBytes,				// ������ ���۵� ����Ʈ
			(PULONG_PTR)&pCompletionKey,	// completion key
			(LPOVERLAPPED *)&pSocketInfo,			// overlapped I/O ��ü
			INFINITE				// ����� �ð�
		);

		if (bResult == false || recvBytes == 0)
		{
			cout << "[INFO][LoginServer] Socket(" << pSocketInfo->socket << ") ���� ����\n" << endl;
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
		// ��Ŷ ����
		int PacketType;
		// Ŭ���̾�Ʈ ���� ������ȭ
		stringstream RecvStream;

		RecvStream << pSocketInfo->dataBuf.buf;
		RecvStream >> PacketType;
		// ��Ŷ ó��


		if (fnProcess[PacketType].funcProcessPacket != nullptr)
		{
			fnProcess[PacketType].funcProcessPacket(RecvStream, pSocketInfo);
		}
		else
		{
			printf_s("[ERROR][LoginServer] ���� ���� ���� ��Ŷ : %d\n", PacketType);
		}
		}
		catch (const std::exception& e)
		{
			printf_s("[ERROR][LoginServer] �� �� ���� ���� �߻� : %s\n", e.what());
		}
		// Ŭ���̾�Ʈ ���
		Recv(pSocketInfo);
	}
}

void IOCPServer::SignUp(stringstream & RecvStream, stSocketInfo * pSocket)
{
	string Id;
	string Pw;

	RecvStream >> Id;
	RecvStream >> Pw;
	cout << "[INFO][LoginServer] ȸ������ �õ� {" << Id << "},{" << Pw << "}\n";

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
	cout << "[INFO][LoginServer] �α��� �õ� {" << Id << "},{" << Pw << "}" << endl;

	#if 1
	CheckLogin = true;
	UserCName = "test\0";
	cout << "[INFO][LoginServer] " << UserCName << " �α��� ����!" << endl;
#else
	CheckLogin = Con.SearchAccount(Id, Pw);

	if (CheckLogin)
	{
		UserCName = Con.GetCName(Id);
		UserCName += "\0";
		cout << "[INFO][LoginServer] " << UserCName << " �α��� ����!" << endl;
	}
	else
		cout << "[INFO][LoginServer] �α��� ����!" << endl;
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
