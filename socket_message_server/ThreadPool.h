#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <vector>
#include <queue>
#include <atomic>


class ThreadPool
{
public:
	ThreadPool(size_t count = 4);
	~ThreadPool();

	//添加任务。
	void AddTask(std::function<void()> task);
	//获取队列中任务个数。
	size_t GetTaskCount();
	//获取空线程的个数。
	int GetAvailableThreadCount();

	void* operator new(std::size_t);
	void operator delete(void*);

private:
	void Work();

private:
	size_t m_Count;//线程个数
	std::mutex m_Mutex;
	std::condition_variable m_ConditionVariable;
	std::vector<std::thread *> m_ThreadVector;
	std::queue<std::function<void()>> m_TaskQueue;
	bool m_IsDestory;

	//空线程的个数。
	std::atomic_int m_AvailableThreadCount;

};


#endif // !__THREAD_POOL_H__