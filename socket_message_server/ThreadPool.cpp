#include "ThreadPool.h"


ThreadPool::ThreadPool(size_t count):m_Count(count), m_IsDestory(false)
{
	if (m_Count < 1)
	{
		m_Count = 1;
	}
	m_AvailableThreadCount = m_Count;
	for (int i = 0; i < m_Count; ++i)
	{
		m_ThreadVector.push_back(new std::thread(&ThreadPool::Work, this));
	}
}


ThreadPool::~ThreadPool()
{
	m_IsDestory = true;
	m_ConditionVariable.notify_all();
	for (std::thread * itemThread : m_ThreadVector)
	{
		itemThread->join();
		delete itemThread;
	}
}


void ThreadPool::Work()
{
	std::function<void()> task;
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_ConditionVariable.wait(lock, [this]()->bool {return (!m_TaskQueue.empty()) || m_IsDestory; });
			if (m_TaskQueue.empty())
			{
				if (m_IsDestory)
				{
					return;
				}
				continue;
			}
			task = std::move(m_TaskQueue.front());
			m_TaskQueue.pop();
		}
		--m_AvailableThreadCount;
		task();
		++m_AvailableThreadCount;
	}
}


void ThreadPool::AddTask(std::function<void()> task)
{
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_TaskQueue.push(std::move(task));
	m_ConditionVariable.notify_all();
}


size_t ThreadPool::GetTaskCount()
{
	return m_TaskQueue.size();
}


int ThreadPool::GetAvailableThreadCount()
{
	return m_AvailableThreadCount;
}

void * ThreadPool::operator new(std::size_t size)
{
	return std::malloc(size);
}

void ThreadPool::operator delete(void * p)
{
	std::free(p);
}
