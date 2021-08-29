#include "DBGroup.h"
#include "DBUser.h"


void DBGroup::GetUserAllGroup(MySql * mySql, int userId, S2CGroupList & ret)
{
	char sql[256] = {};
	std::sprintf(sql, "SELECT `GroupId`,`GroupName`,`AdminId` FROM `Group` WHERE `GroupId` IN (SELECT `GroupId` FROM `GroupUser` WHERE `UserId` = '%d')", userId);
	std::vector<std::vector<std::string>> sqlData;
	mySql->ExecSqlGetResult(sql, sqlData);
	for (std::vector<std::string> & itemSqlData : sqlData)
	{
		S2CGroup groupItem;
		groupItem.GroupId = std::stoi(itemSqlData.at(0));
		memcpy(groupItem.GroupName, itemSqlData.at(1).c_str(), itemSqlData.at(1).size()+1);
		groupItem.AdminId = std::stoi(itemSqlData.at(2));

		std::sprintf(sql, "SELECT `UserId`,`UserName` FROM `User` WHERE `UserId` IN (SELECT `UserId` FROM `GroupUser` WHERE `GroupId` = '%d')", groupItem.GroupId);
		std::vector<std::vector<std::string>> sqlDataUser;
		mySql->ExecSqlGetResult(sql, sqlDataUser);
		DBUser::Vector2UserList(sqlDataUser, groupItem.UserList);
		ret.GroupVector.push_back(groupItem);
	}
}

void DBGroup::GetUserIdByGroupId(MySql * mySql, int groupId, std::vector<int>& ret)
{
	char sql[128] = {};
	std::sprintf(sql, "SELECT `UserId` FROM `GroupUser` WHERE `GroupId` = '%d'", groupId);
	std::vector<std::vector<std::string>> sqlData;
	mySql->ExecSqlGetResult(sql, sqlData);
	for (std::vector<std::string> & itemSqlData : sqlData)
	{
		ret.push_back(std::stoi(itemSqlData.at(0)));
	}
}
