#include "ClientSocket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "Config.h"


const int ClientSocket::SOCKET_CLOSE = -1;


ClientSocket::ClientSocket(struct ClientInfo & clientInfo)
	:m_ClientInfo(clientInfo)
	,m_IsNonBlock(false)
	,m_CurReadBufSize(0)
    ,m_ReadBufMaxSize(CONFIG_CLIENT_READBUF_MAX_SIZE)
    ,m_ReadBuf(new char[CONFIG_CLIENT_READBUF_MAX_SIZE])
	,m_CurWriteBufSize(0)
    ,m_WriteBufMaxSize(CONFIG_CLIENT_WRITEBUF_MAX_SIZE)
    ,m_WriteBuf(new char[CONFIG_CLIENT_WRITEBUF_MAX_SIZE])
	,m_IsWriteing(false)
	,m_CfdIsRead(false)
	,m_CfdIsWrite(false)
{
}


ClientSocket::~ClientSocket()
{
	close(m_ClientInfo.cfd);
	delete[] m_ReadBuf;
	delete[] m_WriteBuf;
}

void ClientSocket::SetNonBlock()
{
	fcntl(m_ClientInfo.cfd, F_SETFL, fcntl(m_ClientInfo.cfd, F_GETFL) | O_NONBLOCK);//非阻塞模式。
	m_IsNonBlock = true;
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
			if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
			{
				//printf("222客户端发送的数据接受完毕，向客户端发送数据\n");
				//char * sendData = "你好客户端,我是epoll边沿非阻塞模式。";
				//send(ta->fd, sendData, strlen(sendData), 0);
			}
			else
			{
				//perror("读数据出错");
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

int ClientSocket::GetFd()
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

