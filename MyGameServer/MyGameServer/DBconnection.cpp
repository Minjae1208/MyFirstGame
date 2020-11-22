#include "DBConnection.h"
#include <iostream>
DBConnection::DBConnection()
{
}

DBConnection::~DBConnection()
{
}

bool DBConnection::Connect(const string& Server, const string& User, const string& Password, const string& Database, const int& Port)
{
	Con = mysql_init(NULL);
	if (!mysql_real_connect(Con, Server.c_str(), User.c_str(), Password.c_str(), Database.c_str(), Port, NULL, 0))
	{
		printf_s("[DB] DB���� ����\n");
		return false;
	}

	return true;
}

void DBConnection::Close()
{
	mysql_close(Con);
}

bool DBConnection::SearchAccount(const string& ID, const string& Password)
{
	bool dResult = false;
	// ������ ���̽� UserAccount���̺��� colunm �˻�
	string sql = "SELECT * FROM UserAccount WHERE ID = '";
	sql += ID + "'and PW = '" + Password + "'";
	if (mysql_query(Con, sql.c_str()))
	{
		printf_s("[DB] �˻� ����\n");
		return false;
	}

	Res = mysql_use_result(Con);
	Row = mysql_fetch_row(Res);

	if (Row != NULL)
		dResult = true;
	else
	{
		printf_s("[ERROR] �ش� ���̵� ����\n");
		dResult = false;
	}
	mysql_free_result(Res);
	return dResult;
}

bool DBConnection::SignUpAccount(const string& ID, const string& Password)
{
	bool dResult = false;

	// ������ ���̽� UserAccount ���̺� ������ ����
	string sql = "INSERT INTO UserAccount (ID, PW) VALUES";
	sql += "('" + ID + "','" + Password + "');";
	if (mysql_query(Con, sql.c_str()))
	{
		printf_s("[DB] �ߺ��� ����\n");
		return false;
	}
	dResult = true;
	return dResult;
}