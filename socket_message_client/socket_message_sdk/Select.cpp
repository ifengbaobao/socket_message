#include "Select.h"

#include <vector>
#include <iostream>


Select::Select()
	: m_Timeout(100)
	, m_IsDestory(false)
	, m_pWorkThread(nullptr)
{
}


Select::~Select()
{
	m_IsDestory = true;
	if (m_pWorkThread != nullptr)
	{
		m_pWorkThread->join();
		delete m_pWorkThread;
	}
}

void Select::Add(int fs, std::shared_ptr<ClientSocket> cs)
{
	m_ClientSocketMap.insert(std::map<SOCKET, std::shared_ptr<ClientSocket> >::value_type(fs, cs));
}

void Select::Remove(int fs)
{
	if (m_ClientSocketMap.count(fs) < 1)
	{
		return;
	}
	std::shared_ptr<ClientSocket> sp = m_ClientSocketMap.at(fs);
	m_ClientSocketMap.erase(fs);
	if (m_SocketCloseCallback)
	{
		m_SocketCloseCallback(sp);
	}
}

void Select::SetSocketCloseCallback(std::function<void(std::shared_ptr<ClientSocket>)> callback)
{
	m_SocketCloseCallback = callback;
}

void Select::SetTimeout(int timeout)
{
	m_Timeout = timeout;
}

void Select::Start()
{
	if (m_pWorkThread == nullptr)
	{
		m_pWorkThread = new std::thread(&Select::Work, this);
	}
}

std::shared_ptr<ClientSocket> Select::GetClientSocket(SOCKET s)
{
	if (m_ClientSocketMap.count(s) == 1)
	{
		return m_ClientSocketMap.at(s);
	}
	return std::shared_ptr<ClientSocket>(nullptr);
}

void Select::Work()
{
	while (!m_IsDestory)
	{
		fd_set readSet;
		fd_set writeSet;
		int maxSocket = -1;
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);

		for (std::map<SOCKET, std::shared_ptr<ClientSocket> >::iterator begin = m_ClientSocketMap.begin();
			begin != m_ClientSocketMap.end(); ++begin)
		{
			if (begin->second->GetCfdIsRead())
			{
				FD_SET(begin->first, &readSet);
			}
			if (begin->second->GetCfdIsWrite())
			{
				FD_SET(begin->first, &writeSet);
			}
			if (maxSocket < begin->first)
			{
				maxSocket = begin->first;
			}
		}

		timeval tm;
		tm.tv_sec = m_Timeout / 1000; //秒
		tm.tv_usec = (m_Timeout % 1000) * 1000;   //微秒

		m_Timeout = -1;

		int count = select(maxSocket + 1, &readSet, &writeSet, NULL, m_Timeout == -1 ? NULL : &tm);

		if (count < 0)
		{
			//std::cout << errno << std::endl;
			//std::cout<<strerror(errno)<<std::endl;
			//发送错误。
		}
		else if (count == 0)
		{
			continue;
		}
		else
		{
			std::vector<SOCKET> closeSocket;
			for (std::map<SOCKET, std::shared_ptr<ClientSocket> >::iterator begin = m_ClientSocketMap.begin();
				begin != m_ClientSocketMap.end(); ++begin)
			{
				if (FD_ISSET(begin->first, &readSet))
				{
					if (ClientSocket::SOCKET_CLOSE == begin->second->Recv())
					{
						//需要关闭。删除对象。
						closeSocket.push_back(begin->first);
					}

				}
				if (FD_ISSET(begin->first, &writeSet))
				{
					if (ClientSocket::SOCKET_CLOSE == begin->second->Send())
					{
						//需要关闭。删除对象。
						closeSocket.push_back(begin->first);
					}
				}

			}
			//有关闭的socket处理一下。
			for (SOCKET s : closeSocket)
			{
				Remove(s);
			}

		}
	}
}
