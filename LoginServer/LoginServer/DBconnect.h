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

	// DB종료
	void Close();

	// 계정 찾기
	bool SearchAccount(const string& ID, const string& Password);

	// 유저 계정 등록
	bool SignUpAccount(const string& ID, const string& Password);

	// 유저 닉네임 가져오기
	string GetCName(const string& ID);

private:
	MYSQL  *Con; // 연결
	MYSQL_RES* Res; // 결과값
	MYSQL_ROW Row; //결과 row ?

};