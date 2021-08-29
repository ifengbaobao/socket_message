#include "ClientSocket.h"

#include <iostream>

#include "SocketMessageSdkConfig.h"


const int ClientSocket::SOCKET_CLOSE = -1;

#ifdef _WIN32
std::atomic_int ClientSocket::s_ClientSocketCount = {0};
#endif

ClientSocket::ClientSocket(struct ClientInfo && clientInfo)
	:m_ClientInfo(clientInfo)
	,m_IsNonBlock(false)
	,m_CurReadBufSize(0)
    ,m_ReadBufMaxSize(CONFIG_CLIENT_READBUF_MAX_SIZE)
    ,m_ReadBuf(new char[CONFIG_CLIENT_READBUF_MAX_SIZE])
	,m_CurWriteBufSize(0)
    ,m_WriteBufMaxSize(CONFIG_CLIENT_WRITEBUF_MAX_SIZE)
    ,m_WriteBuf(new char[CONFIG_CLIENT_WRITEBUF_MAX_SIZE])
	,m_IsWriteing(false)
	,m_CfdIsRead(true)
	,m_CfdIsWrite(false)
{
#ifdef _WIN32
	if (s_ClientSocketCount == 0)
	{
		++s_ClientSocketCount;
		//启动windows socket 2.x 环境
		WORD ver = MAKEWORD(2, 2);
		WSADATA data = {};
		WSAStartup(ver, &data);
	}
#endif
}

ClientSocket::~ClientSocket()
{
#ifdef _WIN32
	if (INVALID_SOCKET != m_ClientInfo.cfd)
	{
		closesocket(m_ClientInfo.cfd);
	}

	--s_ClientSocketCount;
	if (s_ClientSocketCount < 1)
	{
		WSACleanup();
	}
#else
	if (INVALID_SOCKET != m_ClientInfo.cfd)
	{
		close(m_ClientInfo.cfd);
	}
#endif
	delete[] m_ReadBuf;
	delete[] m_WriteBuf;
}

bool ClientSocket::SetNonBlock(bool isNonBlock)
{
	if (isNonBlock == m_IsNonBlock)
	{
		return true;
	}
	m_IsNonBlock = isNonBlock;
	return SetNonBlockOperation(isNonBlock);

}

int ClientSocket::Recv()
{
	std::unique_lock<std::mutex> lock(m_MutexRead);
	do {
		int readCount = recv(m_ClientInfo.cfd, m_ReadBuf + m_CurReadBufSize, m_ReadBufMaxSize - m_CurReadBufSize, 0);
		if (readCount > 0)
		{
			//接收客户端数据成功。
			m_CurReadBufSize += readCount;
			if (m_ReadDataCallback)
			{
				int eatCount = m_ReadDataCallback(this,m_ReadBuf, m_CurReadBufSize);
				if (eatCount > 0)
				{
					memmove(m_ReadBuf, m_ReadBuf + eatCount, m_CurReadBufSize - eatCount);
					m_CurReadBufSize -= eatCount;
				}
				if (m_CurReadBufSize == m_ReadBufMaxSize)
				{
					//说明缓冲区读满了出错了。

				}
			}
			else {
				//没有处理数据的回调。

			}
		}
		else if (readCount == 0)//数据接受完毕
		{
			return SOCKET_CLOSE;
		}
		else
		{
#ifdef _WIN32
			if (WSAGetLastError() == WSAEWOULDBLOCK)
#else
			if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
#endif
			{
				//printf("222客户端发送的数据接受完毕，向客户端发送数据\n");
				//char * sendData = "你好客户端,我是epoll边沿非阻塞模式。";
				//send(ta->fd, sendData, strlen(sendData), 0);
			}
			else
			{
				std::cout << errno << "    读数据出错" << std::endl;
				return SOCKET_CLOSE;
			}
			break;
		}
	} while (m_IsNonBlock);
	return 0;
}

void ClientSocket::SetReadDataCallback(std::function<int(ClientSocket*,char*, int)> callback)
{
	m_ReadDataCallback = callback;
}

void ClientSocket::SetWriteDataCallback(std::function<void(ClientSocket*, bool)> callback)
{
	m_WriteDataCallback = callback;
}

SOCKET ClientSocket::GetFd()
{
	return m_ClientInfo.cfd;
}

unsigned short ClientSocket::GetPort()
{
	return m_ClientInfo.port;
}

char * ClientSocket::GetIp()
{
	return m_ClientInfo.ip;
}

int ClientSocket::GetSize4EmptyWrite()
{
	return m_WriteBufMaxSize - m_CurWriteBufSize;
}

std::atomic_bool & ClientSocket::GetCfdIsRead()
{
	return m_CfdIsRead;
}

std::atomic_bool & ClientSocket::GetCfdIsWrite()
{
	return m_CfdIsWrite;
}

bool ClientSocket::CreateSocket()
{
	if (INVALID_SOCKET != m_ClientInfo.cfd)
	{
		return true;
	}
	m_ClientInfo.cfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_ClientInfo.cfd)
	{
		printf("新建socket失败");
		return false;
	}
	return true;
}

bool ClientSocket::Connect(const char * addr, unsigned short port, int timeout)
{
	if (INVALID_SOCKET == m_ClientInfo.cfd)
	{
		if (!CreateSocket())
		{
			return false;
		}
	}
	sockaddr_in socketAddr = {};
	socketAddr.sin_family = AF_INET;
    inet_pton(AF_INET, addr, (void*)&socketAddr.sin_addr);
	socketAddr.sin_port = htons(port);
	SetNonBlockOperation(true);
	bool ret = true;
	do {
		if (0 != connect(m_ClientInfo.cfd, (const sockaddr*)&socketAddr, sizeof(sockaddr)))
		{
			fd_set set;
			FD_ZERO(&set);
			FD_SET(m_ClientInfo.cfd, &set);
			timeval tm;
			tm.tv_sec = timeout / 1000; //秒
			tm.tv_usec = (timeout % 1000) * 1000;   //微秒
			if (select(m_ClientInfo.cfd + 1, NULL, &set, NULL, timeout == -1 ? NULL : &tm) <= 0)
			{
				//失败或超时
				//("connect timeout or error!\n");
				ret = false;
			}
		}
	} while (false);

	if (!m_IsNonBlock)
	{
		SetNonBlockOperation(false);
	}
	return ret;
}

//写数据。
int ClientSocket::Write(char * pData, int size)
{
	{
		std::unique_lock<std::mutex> lock(m_MutexWrite);
		if (size > m_WriteBufMaxSize - m_CurWriteBufSize)
		{
			return 0;
		}
		memcpy(m_WriteBuf + m_CurWriteBufSize, pData, size);
		m_CurWriteBufSize += size;
	}
	return Send();
}

//发送数据。
int ClientSocket::Send()
{
	std::unique_lock<std::mutex> lock(m_MutexWrite);
	bool hasData = false;
	do {
		if (m_CurWriteBufSize == 0)
		{
			hasData = false;
			break;
		}
		int overCount = send(m_ClientInfo.cfd, m_WriteBuf, m_CurWriteBufSize, 0);
		if (overCount > 0)
		{
			if (m_CurWriteBufSize == overCount)
			{
				m_CurWriteBufSize = 0;
				hasData = false;
				break;
			}
			//数据仅仅发送了一部分。
			memmove(m_WriteBuf, m_WriteBuf + overCount, m_CurWriteBufSize - overCount);
			m_CurWriteBufSize -= overCount;
		}
		else if (overCount == 0)
		{
			return SOCKET_CLOSE;
		}
		else
		{
			if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
			{
				hasData = true;
				break;
			}
			else
			{
				//perror("写数据出错");
				return SOCKET_CLOSE;
			}
		}
	} while (m_IsNonBlock);
	if (m_WriteDataCallback && m_IsWriteing != hasData)
	{
		m_IsWriteing = hasData;
		m_WriteDataCallback(this, hasData);
	}
	return 0;
}

//设置非阻塞模式。
bool ClientSocket::SetNonBlockOperation(bool isNonBlock)
{
	if (INVALID_SOCKET == m_ClientInfo.cfd)
	{
		return false;
	}
#ifdef WIN32
	unsigned long ul = isNonBlock ? 1 : 0;
	// FIONBIO 阻塞模式
	//ul =0 表示阻塞模式 
	ioctlsocket(m_ClientInfo.cfd, FIONBIO, &ul);
#else
	//操作文件描述法,先获取属性
	int flags = fcntl(m_ClientInfo.cfd, F_GETFL, 0);
	if (flags < 0)
	{
		return false; //出错了
	}
	if (isNonBlock)
	{
		//非阻塞模式
		flags = flags | O_NONBLOCK;
	}
	else
	{
		//阻塞模式
		flags = flags & ~O_NONBLOCK;
	}
	if (fcntl(m_ClientInfo.cfd, F_SETFL, flags) != 0)
	{
		return false;
	}
#endif
	return true;
}
