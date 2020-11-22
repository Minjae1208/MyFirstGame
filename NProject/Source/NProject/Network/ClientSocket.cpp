// Fill out your copyright notice in the Description page of Project Settings.


#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ClientSocket.h"
#include <sstream>
#include "ClientNetwork.h"

ClientSocket::ClientSocket() :
	StopTaskCounter(0),
	Thread(nullptr)
{
}

ClientSocket::~ClientSocket()
{
	StopListen();

	closesocket(ServerSocket);
	WSACleanup();
}

bool ClientSocket::InitSocket()
{
	WSADATA wasData;

	int nRet = WSAStartup(MAKEWORD(2, 2), &wasData);
	if (nRet != 0)
	{
		return false;
	}

	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET) 
	{
		return false;
	}
	return true;
}

bool ClientSocket::Connect(const char * SIP, int SPORT)
{
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;

	stServerAddr.sin_port = htons(SPORT);
	stServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int nRet = connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr));
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}

bool ClientSocket::SendPacket(const stringstream& buffer)
{
	return send(ServerSocket, (CHAR*)buffer.str().c_str(), buffer.str().length(), 0) != -1;
}

void ClientSocket::CloseSocket()
{
	closesocket(ServerSocket);
	WSACleanup();
}

bool ClientSocket::Init()
{
	return true;
}

uint32 ClientSocket::Run()
{
	FPlatformProcess::Sleep(0.03);

	#if 0
	AsyncTask(ENamedThreads::GameThread, [this]() {
	while (StopTaskCounter.GetValue() == 0)
	{
		char recvBuffer[MAX_BUFFER];
		int nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);
		if (nRecvLen > 0)
		{

			stringstream RecvStream;
			RecvStream << recvBuffer;

			int PacketType;
			RecvStream >> PacketType;
			ClientNetwork::Recv(PacketType, RecvStream);

		}
	}
	}); //AsyncTask
	#else
	while (StopTaskCounter.GetValue() == 0)
	{
		char recvBuffer[MAX_BUFFER];
		int nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);
		if (nRecvLen > 0)
		{
			AsyncTask(ENamedThreads::GameThread, [recvBuffer]() {
				int PacketType;
				stringstream RecvStream;
				RecvStream << recvBuffer;

				RecvStream >> PacketType;
				ClientNetwork::Recv(PacketType, RecvStream);
			});
		}
	}
	#endif
	

	return 0;
}

void ClientSocket::Stop()
{
	StopTaskCounter.Increment();
}

void ClientSocket::Exit()
{
	
}

bool ClientSocket::StartListen()
{
	if (Thread != nullptr) return false;
	Thread = FRunnableThread::Create(this, TEXT("ClientSocket"), 0, TPri_BelowNormal);
	return (Thread != nullptr);
}

void ClientSocket::StopListen()
{
	if (Thread != nullptr)
	{
		//Thread->WaitForCompletion();
		Thread->Kill();
		delete Thread;
	}
	Thread = nullptr;
}
