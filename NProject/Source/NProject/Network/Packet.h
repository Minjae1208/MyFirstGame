#pragma once

#include <sstream>
#include <string>
#include <map>

enum EServerType
{
	LOGIN_SERVER,
	INGAME_SERVER,

	Max
};

// CS : Client -> Server
// SC : Server -> Client
enum EPacketType
{
	// Login Server
	CS_SIGNUP,				// 회원 가입
	SC_SIGNUP,

	CS_LOGIN_SERVER,
	SC_LOGIN_SERVER,

	// Ingame Server
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

class ReqPacket_Login
{
public:
	std::string strID;
	std::string strPW;

	friend std::ostream& operator<<(std::ostream& stream, ReqPacket_Login& packet)
	{
		stream << packet.strID << std::endl;
		stream << packet.strPW << std::endl;

		return stream;
	}

	friend std::istream& operator>>(std::istream& stream, ReqPacket_Login& packet)
	{
		stream >> packet.strID;
		stream >> packet.strPW;

		return stream;
	}
};

class RecvPacket_Login
{
public:
	bool LoginResult;
	std::string UserCName;

	friend std::ostream& operator<<(std::ostream& stream, RecvPacket_Login& packet)
	{
		stream << packet.LoginResult << std::endl;
		if (packet.LoginResult)
			stream << packet.UserCName << std::endl;

		return stream;
	}

	friend std::istream& operator>>(std::istream& stream, RecvPacket_Login& packet)
	{
		stream >> packet.LoginResult;
		if (packet.LoginResult)
			stream >> packet.UserCName;

		return stream;
	}
};

class RecvPacket_Ingame
{
public:
	bool Result;
	int SessionID;

	friend std::ostream& operator<<(std::ostream& stream, RecvPacket_Ingame& packet)
	{
		stream << packet.Result << std::endl;
		if (packet.Result)
			stream << packet.SessionID << std::endl;

		return stream;
	}

	friend std::istream& operator>>(std::istream& stream, RecvPacket_Ingame& packet)
	{
		stream >> packet.Result;
		if (packet.Result)
			stream >> packet.SessionID;

		return stream;
	}
};

class Packet_MoveInfo
{
public:
	Packet_MoveInfo() {};
	~Packet_MoveInfo() {};

	int SessionID;

	float X;
	float Y;
	float Z;

	float Yaw;
	float Pitch;
	float Roll;

	float VX;
	float VY;
	float VZ;

	friend std::ostream& operator<<(std::ostream &stream, Packet_MoveInfo& info)
	{
		stream << info.SessionID << std::endl;
		stream << info.X << std::endl;
		stream << info.Y << std::endl;
		stream << info.Z << std::endl;
		stream << info.VX << std::endl;
		stream << info.VY << std::endl;
		stream << info.VZ << std::endl;
		stream << info.Yaw << std::endl;
		stream << info.Pitch << std::endl;
		stream << info.Roll << std::endl;

		return stream;
	}
	friend std::istream& operator >> (std::istream& stream, Packet_MoveInfo& info)
	{
		stream >> info.SessionID;
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

class PacketCharactersInfo
{
public:
	PacketCharactersInfo() {};
	~PacketCharactersInfo() {};

	std::map<int, Packet_MoveInfo> players;

	friend std::ostream& operator<<(std::ostream &stream, PacketCharactersInfo& info)
	{
		stream << info.players.size() << std::endl;
		for (auto& kvp : info.players)
		{
			stream << kvp.first << std::endl;
			stream << kvp.second << std::endl;
		}

		return stream;
	}

	friend std::istream& operator>>(std::istream &stream, PacketCharactersInfo& info)
	{
		int nPlayers = 0;
		int SessionId = 0;
		Packet_MoveInfo Player;
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
