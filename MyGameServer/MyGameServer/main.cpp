#include "stdafx.h"
#include "IOCPServer.h"

int main()
{
	IOCPServer iocp_server;
	if (iocp_server.Init())
	{
		iocp_server.StartServer();
	}
	return 0;
}