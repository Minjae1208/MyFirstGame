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
	CS_SIGNUP,				// 회원 가입
	SC_SIGNUP,

	CS_LOGIN_SERVER,
	SC_LOGIN_SERVER,

	//------- Ingame Server
	CS_INGAME_SERVER,
	SC_INGAME_SERVER,

	CS_ENTER_ZONE,			// 인게임 맵 입장
	SC_ENTER_ZONE,

	CS_MOVE_INFO,			// 이동 동기화
	SC_MOVE_INFO,

	CS_CHAT,				// 채팅
	SC_CHAT,

	SC_ENTER_NEW_PLAYER,	// 새로운 플레이어 입장

	CS_LOGOUT_PLAYER,		// 로그 아웃
};

class COMMON_API Common
{
public:
	Common();
	~Common();
};