#pragma once

#ifdef COMMONCLASS_EXPORTS
#define COMMONCLASS_API __declspec(dllexport)
#else
#define COMMONCLASS_API __declspec(dllimport)
#endif

#include <iostream>
#include <map>

using namespace std;

#define MAX_CLIENTS 20

enum COMMONCLASS_API EPacketType
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

class COMMONCLASS_API cCharacter {
public:
	cCharacter();
	~cCharacter();

	// ���� ���̵�
	int		SessionId;
	// ��ġ
	float	X;
	float	Y;
	float	Z;
	// ȸ����
	float	Yaw;
	float	Pitch;
	float	Roll;
	// �ӵ�
	float VX;
	float VY;
	float VZ;

	friend ostream& operator<<(ostream &stream, cCharacter& info)
	{
		stream << info.SessionId << endl;
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.VX << endl;
		stream << info.VY << endl;
		stream << info.VZ << endl;
		stream << info.Yaw << endl;
		stream << info.Pitch << endl;
		stream << info.Roll << endl;

		return stream;
	}

	friend istream& operator>>(istream& stream, cCharacter& info)
	{
		stream >> info.SessionId;
		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;
		stream >> info.VX;
		stream >> info.VY;
		stream >> info.VZ;
		stream >> info.Yaw;
		stream >> info.Pitch;
		stream >> info.Roll;

		return stream;
	}
};

class COMMONCLASS_API cCharactersInfo
{
public:
	cCharactersInfo();
	~cCharactersInfo();
	
	map<int, cCharacter> players;

	friend ostream& operator<<(ostream &stream, cCharactersInfo& info)
	{
		stream << info.players.size() << endl;
		for (auto& kvp : info.players)
		{
			stream << kvp.first << endl;
			stream << kvp.second << endl;
		}

		return stream;
	}

	friend istream &operator>>(istream &stream, cCharactersInfo& info)
	{
		int nPlayers = 0;
		int SessionId = 0;
		cCharacter Player;
		info.players.clear();

		stream >> nPlayers;
		for (int i = 0; i < nPlayers; i++)
		{
			stream >> SessionId;
			stream >> Player;
			info.players[SessionId] = Player;			
		}

		return stream;
	}
};

class COMMONCLASS_API CommonClass
{
public:
	CommonClass();
	~CommonClass();
};

