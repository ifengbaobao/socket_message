#ifndef __MY_SQL_POOL_H__
#define __MY_SQL_POOL_H__

#include <vector>
#include <mutex>
#include <condition_variable>

#include "MySql.h"


class MySqlPool
{
public:
	explicit MySqlPool(int count);
	~MySqlPool();

	bool Init(char * server, char * user, char * password, char* database, int port);

	//移除末尾元素。
	MySql * Pop();
	void Push(MySql* mySql);

	//得到可用的个数。
	int GetAvailableCount();
	//得到一共多少个连接。
	int GetCount();

private:
	int m_Count;
	std::vector<MySql*> m_MySqlVector;
	std::mutex m_Mutex;
	std::condition_variable m_ConditionVariable;
};


#endif // !__MY_SQL_POOL_H__
