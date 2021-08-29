#include "MySqlPool.h"


MySqlPool::MySqlPool(int count)
{
	if (count <= 0)
	{
		count = 1;
	}
	m_Count = count;
	for (int i = 0; i < count; ++i)
	{
		m_MySqlVector.push_back(new MySql());
	}
}

MySqlPool::~MySqlPool()
{
	for (MySql * & item : m_MySqlVector)
	{
		delete item;
	}
}

bool MySqlPool::Init(char * server, char * user, char * password, char * database, int port)
{
	bool ret = true;
	for (MySql * & item: m_MySqlVector)
	{
		if (!item->Init(server, user, password, database, port))
		{
			ret = false;
		}
	}
	return ret;
}

MySql * MySqlPool::Pop()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		m_ConditionVariable.wait(lock, [this]()->bool {return (!this->m_MySqlVector.empty()); });
		if (m_MySqlVector.empty())
		{
			continue;
		}
		MySql * ret = m_MySqlVector.back();
		m_MySqlVector.pop_back();
		return ret;
	}
}

void MySqlPool::Push(MySql * mySql)
{
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_MySqlVector.push_back(std::move(mySql));
	m_ConditionVariable.notify_all();
}

int MySqlPool::GetAvailableCount()
{
	return m_MySqlVector.size();
}

int MySqlPool::GetCount()
{
	return m_Count;
}
