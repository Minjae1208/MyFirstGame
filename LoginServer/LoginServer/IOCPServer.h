#pragma once

#define _WINSOCK_DEPREACTED_NO_WARNINGS

#include <WinSock2.h>
#include <map>
#include <vector>
#include <iostream>
#include "Common.h"
#include "DBConnect.h"
#include "IOCPBase.h"

using namespace std;

#define DB_ADDRESS "localhost"
#define DB_PORT 3306
#define DB_ID "root"
#define DB_PW "chaminjae123@"
#define DB_SCHEMA "3Dgameuser"

class IOCPServer : public IOCPBase
{
public:
	IOCPServer();
	virtual ~IOCPServer();

	virtual void StartLoginServer() override;

	virtual bool CreateWorkerThread() override;

	virtual void WorkerThread() override;

	static void Send(stSocketInfo* pSockert);

private:
	static map<int, SOCKET> SessionSocket;
	static DBConnect Con;

	FuncProcess				fnProcess[100];	// ��Ŷ ó�� ����ü

	// ȸ������
	static void SignUp(stringstream & RecvStream, stSocketInfo * pSocket);
	// DB�� �α���
	static void Login(stringstream & RecvStream, stSocketInfo * pSocket);
	// ĳ���� �ʱ� ���
	static void Exit(stringstream & RecvStream, stSocketInfo * pSocket);
};

