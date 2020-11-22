// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <map>

#include "Packet.h"

using namespace std;

// Server Data
#define MAX_BUFFER 4096

#define SERVER_IP 127.0.0.1
#define MAX_CLIENTS 10

//Socket Struct
struct stSOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes;
};



class NPROJECT_API ClientSocket : public FRunnable
{
public:
	ClientSocket();
	virtual ~ClientSocket();

	bool InitSocket();

	bool Connect(const char* SIP, int SPORT);

	bool SendPacket(const stringstream& buffer);

	void CloseSocket();

	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;

	//~~ FRunnable
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	//~~

	bool StartListen();
	void StopListen();

private :
	SOCKET ServerSocket;

};
