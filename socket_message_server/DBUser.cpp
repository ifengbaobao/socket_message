#include "DBUser.h"

#include <vector>


bool DBUser::Login(MySql * mySql, const C2SLogin & login, S2CLogin & ret)
{
	char sql[128] = {};
	std::string passwordFrom(login.Password);
	std::string password;
	mySql->EscapeString(passwordFrom, password);
	std::sprintf(sql, "SELECT `UserId`,`UserName` FROM `User` WHERE `UserId` = '%d' AND `Password` = '%s'", login.UserId, password.c_str());
	std::vector<std::vector<std::string>> sqlData;
	mySql->ExecSqlGetResult(sql, sqlData);
	if (sqlData.empty())
	{
		ret.UserId = -1;
		const char * err = "用户名密码错误";
		memcpy(ret.UserName, err, strlen(err) + 1);
		return false;
	}
	else
	{
		ret.UserId = std::stoi(sqlData.at(0).at(0));
		std::string & username = sqlData.at(0).at(1);
		memcpy(ret.UserName, username.c_str(), username.size()+1);
		return true;
	}
}

void DBUser::AllUserList(MySql * mySql, S2CUserList & ret)
{
	char sql[128] = "SELECT `UserId`,`UserName` FROM `User`";
	std::vector<std::vector<std::string>> sqlData;
	mySql->ExecSqlGetResult(sql, sqlData);
	Vector2UserList(sqlData, ret);
}

void DBUser::Vector2UserList(std::vector<std::vector<std::string>> sqlData, S2CUserList & ret)
{
	for (std::vector<std::string> & itemSqlData : sqlData)
	{
		S2CUser user;
		user.UserId = std::stoi(itemSqlData.at(0));
		std::string & username = itemSqlData.at(1);
		memcpy(user.UserName, username.c_str(), username.size() + 1);
		ret.UserVector.push_back(user);
	}
}
