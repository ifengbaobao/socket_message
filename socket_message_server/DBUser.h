#ifndef __DB_USER_H__
#define __DB_USER_H__


#include "DataType.h"
#include "MySql.h"


class DBUser
{
private:
	DBUser() = delete;
	DBUser(const DBUser&) = delete;
	DBUser & operator=(DBUser&) = delete;
	~DBUser() = delete;

public:
	static bool Login(MySql* mySql, const C2SLogin& login, S2CLogin & ret);

	//获取全部用户列表。
	static void AllUserList(MySql* mySql,S2CUserList & ret);

	static void Vector2UserList(std::vector<std::vector<std::string>> sqlData, S2CUserList & ret);

};


#endif // __DB_USER_H__
