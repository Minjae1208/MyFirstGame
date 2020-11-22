#pragma once


#ifdef COMMON_EXPORTS
#define COMMON_API __declspec(dllexport)
#else
#define COMMON_API __declspec(dllimport)
#endif

#include <iostream>
#include <map>

using namespace std;

#define MAX_CLIENTS 20

// CS : Client -> Server
// SC : Server -> Client
enum COMMON_API EPacketType
{
	//------- Login Server
	CS_SIGNUP,				// ȸ�� ����
	SC_SIGNUP,

	CS_LOGIN_SERVER,
	SC_LOGIN_SERVER,

	//------- Ingame Server
	CS_INGAME_SERVER,
	SC_INGAME_SERVER,

	CS_ENTER_ZONE,			// �ΰ��� �� ����
	SC_ENTER_ZONE,

	CS_MOVE_INFO,			// �̵� ����ȭ
	SC_MOVE_INFO,

	CS_CHAT,				// ä��
	SC_CHAT,

	SC_ENTER_NEW_PLAYER,	// ���ο� �÷��̾� ����

	CS_LOGOUT_PLAYER,		// �α� �ƿ�
};

class COMMON_API Common
{
public:
	Common();
	~Common();
};