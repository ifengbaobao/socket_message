#ifndef __DB_GROUP_H__
#define __DB_GROUP_H__


#include "DataType.h"
#include "MySql.h"

class DBGroup
{
private:
	DBGroup() = delete;
	DBGroup(const DBGroup &) = delete;
	DBGroup & operator=(DBGroup&) = delete;
	~DBGroup() = delete;

public:

	//获取指定用户的所有的群的信息。
	static void GetUserAllGroup(MySql * mySql,int userId, S2CGroupList & ret);

	//获取指定群的所有的用户。
	static void GetUserIdByGroupId(MySql * mySql, int groupId,std::vector<int>& ret);


};


#endif // !__DB_GROUP_H__
