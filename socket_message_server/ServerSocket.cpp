#include "ServerSocket.h"


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "Logger.hpp"

ServerSocket::ServerSocket() :m_Sfd(-1),m_IsNonBlock(false)
{
}

ServerSocket::~ServerSocket()
{
	if (m_Sfd != -1)
	{
		close(m_Sfd);
	}
}

bool ServerSocket::Init(unsigned short port, int listenQueuedCount)
{
	int sfd = -1;
	do {
		sfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sfd == -1)
		{
			//perror("socket err ");
			LOGE("socket 错误");
			break;
		}

		struct sockaddr_in add;
		add.sin_family = AF_INET;
		add.sin_port = htons(port);
		add.sin_addr.s_addr = INADDR_ANY;
		int ret = bind(sfd, (const struct sockaddr*)&add, sizeof(add));
		if (ret == -1)
		{
			LOGE("bind 错误");
			break;
		}
		ret = listen(sfd, listenQueuedCount);
		if (ret == -1)
		{
			LOGE("listen 错误");
			break;
		}
		m_Sfd = sfd;
		return true;
	} while (0);
	if (sfd != -1)
	{
		close(sfd);
	}
	return false;
}

void ServerSocket::SetNonBlock()
{
	if (m_Sfd != -1)
	{
		fcntl(m_Sfd, F_SETFL, fcntl(m_Sfd, F_GETFL) | O_NONBLOCK);//非阻塞模式。
		m_IsNonBlock = true;
	}
}

void ServerSocket::Accept()
{
	struct sockaddr_in cliAddr = { 0 };
	int cliAddrLen = sizeof(struct sockaddr_in);
	do{
		int cfd = accept(m_Sfd, (struct sockaddr*)&cliAddr, (socklen_t *)&cliAddrLen);
		if (cfd > 0)
		{
			ClientInfo cInfo;
			cInfo.cfd = cfd;
			cInfo.port = ntohs(cliAddr.sin_port);
			inet_ntop(AF_INET, &cliAddr.sin_addr.s_addr, cInfo.ip, sizeof(cInfo.ip));
			if (m_ClientConnectCallback)
			{
				m_ClientConnectCallback(cInfo);
			}
		}
		else {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				//printf("111没有新的连接了%d\n", cfd);
			}
			else {
				printf("111没有新连接了%d,粗错了\n", cfd);
				perror("accept err");
			}
			break;
		}
	} while (m_IsNonBlock);
}

void ServerSocket::SetClientConnectCallback(std::function<void(struct ClientInfo&)> callback)
{
	m_ClientConnectCallback = callback;
}

int ServerSocket::GetFd()
{
	return m_Sfd;
}


