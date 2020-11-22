#include "DBconnect.h"
#include <iostream>
DBConnect::DBConnect()
{
}

DBConnect::~DBConnect()
{
}


bool DBConnect::Connect(const string& Server, const string& User, const string& Password, const string& Database, const int& Port)
{
	Con = mysql_init(NULL);
	if (!mysql_real_connect(Con, Server.c_str(), User.c_str(), Password.c_str(), Database.c_str(), Port, NULL, 0))
	{
		printf_s("[DB] DB접속 실패\n");
		return false;
	}
	mysql_set_character_set(Con, "euckr");
	return true;
}


void DBConnect::Close()
{
	mysql_close(Con);
}


bool DBConnect::SearchAccount(const string& ID, const string& Password)
{
	bool dResult = false;
	// 데이터 베이스 UserAccount테이블의 colunm 검사
	string sql = "SELECT * FROM UserAccount WHERE ID = '";
	sql += ID + "'and PW = '" + Password + "'";
	if (mysql_query(Con, sql.c_str()))
	{
		printf_s("[DB] 검색 실패\n");
		return false;
	}

	Res = mysql_use_result(Con);
	Row = mysql_fetch_row(Res);

	if (Row != NULL)
		dResult = true;
	else
	{
		printf_s("[ERROR] 해당 아이디 없음\n");
		dResult = false;
	}
	mysql_free_result(Res);
	return dResult;
}

bool DBConnect::SignUpAccount(const string& ID, const string& Password)
{
	bool dResult = false;

	// 데이터 베이스 UserAccount 테이블에 데이터 삽입
	string sql = "INSERT INTO UserAccount (ID, PW) VALUES";
	sql += "('" + ID + "','" + Password + "');";
	if (mysql_query(Con, sql.c_str()))
	{
		printf_s("[DB] 중복된 계정\n");
		return false;
	}
	dResult = true;
	return dResult;
}
string DBConnect::GetCName(const string & ID)
{
	string sql = "SELECT * FROM UserAccount WHERE ID = '";
	sql += ID + "'";


	if(mysql_query(Con, sql.c_str()))
	{
		printf_s("[DB] 검색 실패\n");
		return false;
	}
	
	Res = mysql_store_result(Con);
	Row = mysql_fetch_row(Res);
	mysql_free_result(Res);

	string CName(Row[2]);
	return CName;

}
