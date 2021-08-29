#include "EPoll.h"

#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include "Logger.hpp"


EPoll::EPoll(int eventSize) :m_Epfd(-1)
{
	if (eventSize < 1)
	{
		eventSize = 1;
	}
	//1，创建epoll树 epoll_create。
	int epfd = epoll_create(eventSize);
	if (epfd == -1)
	{
		LOGE("epoll 创建失败");
		return;
	}
	m_Epfd = epfd;
	m_EventSize = eventSize;
	m_pEvents = new struct epoll_event[m_EventSize];

}


EPoll::~EPoll()
{
	if (m_Epfd != -1)
	{
		close(m_Epfd);
		m_Epfd = -1;
		delete[] m_pEvents;
		m_pEvents = nullptr;
	}
}

//参数，单位毫秒。
void EPoll::Wait(int timeout)
{
	if (m_Epfd == -1)
	{
		return;
	}
	int epollChangeCount = epoll_wait(m_Epfd, m_pEvents, m_EventSize, timeout);
	if (m_EventCallback)
	{
		for (int i = 0; i < epollChangeCount && m_pEvents != nullptr; ++i)
		{
			bool isReadEvent = m_pEvents[i].events & EPOLLIN;
			bool isWriteEvent = m_pEvents[i].events & EPOLLOUT;
			m_EventCallback(m_pEvents[i].data.ptr, isReadEvent, isWriteEvent);
		}
		if (epollChangeCount == -1)
		{
			int err = errno;
			std::cout << err << std::endl;
		}
	}
}

//添加
void EPoll::Add(int fs, void * data, bool isWrite, bool isRead)
{
	struct epoll_event ee = { 0 };
	if (isRead)
	{
		ee.events |= EPOLLIN;
	}
	if (isWrite)
	{
		ee.events |= EPOLLOUT;
	}
	//非阻塞模式。
	if (fcntl(fs, F_GETFL) & O_NONBLOCK)
	{
		ee.events |= EPOLLET;//设置为边沿模式。
	}
	ee.data.ptr = data;
	epoll_ctl(m_Epfd, EPOLL_CTL_ADD, fs, &ee);
}

//删除
void EPoll::Remove(int fs)
{
	epoll_ctl(m_Epfd, EPOLL_CTL_DEL, fs, NULL);
}

//修改
void EPoll::Change(int fs, void * data, bool isWrite, bool isRead)
{
	struct epoll_event ee = { 0 };
	if (isRead)
	{
		ee.events |= EPOLLIN;
	}
	if (isWrite)
	{
		ee.events |= EPOLLOUT;
	}
	//非阻塞模式。
	if (fcntl(fs, F_GETFL) & O_NONBLOCK)
	{
		ee.events |= EPOLLET;//设置为边沿模式。
	}
	ee.data.ptr = data;
	epoll_ctl(m_Epfd, EPOLL_CTL_MOD, fs, &ee);
}

void EPoll::SetEventCallback(std::function<void(void*, bool, bool)> callback)
{
	m_EventCallback = callback;
}