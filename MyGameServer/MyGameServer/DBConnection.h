#pragma once

#include <mysql.h>
#include <string>

using namespace std;

class DBConnection
{
public :
		DBConnection();
		~DBConnection();

		bool Connect(
			const string& Server,
			const string& User,
			const string& Password,
			const string& Database,
			const int& Port
		);

		// DB����
		void Close();

		// ���� ã��
		bool SearchAccount(const string& ID, const string& Password);

		// ���� ���� ���
		bool SignUpAccount(const string& ID, const string& Password);

private :
	MYSQL  *Con; // ����
	MYSQL_RES* Res; // �����
	MYSQL_ROW Row; //��� row ?

};