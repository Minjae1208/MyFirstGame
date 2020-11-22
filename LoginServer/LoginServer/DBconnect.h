#pragma once
#pragma comment(lib, "libmySQL.lib")

#include <mysql.h>
#include <string>

using namespace std;

class DBConnect
{
public:
	DBConnect();
	~DBConnect();

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

	// ���� �г��� ��������
	string GetCName(const string& ID);

private:
	MYSQL  *Con; // ����
	MYSQL_RES* Res; // �����
	MYSQL_ROW Row; //��� row ?

};