#ifndef __SERVER_SOCKET_H__
#define __SERVER_SOCKET_H__

#include <unistd.h>
#include <functional>

#include "ClientSocket.h"


class ServerSocket
{
public:
	ServerSocket();
	ServerSocket(const ServerSocket &) = delete;
	const ServerSocket& operator=(const ServerSocket& good) = delete;
	virtual ~ServerSocket();

	bool Init(unsigned short port, int listenQueuedCount = 128);

	//设置非阻塞模式。
	void SetNonBlock();

	//接收连接，
	void Accept();

	//设置客户端有新连接的回调。
	void SetClientConnectCallback(std::function<void(struct ClientInfo &)> callback);

	//获取文件描述符。
	int GetFd();

private:
	//socket 的文件描述符。
	int m_Sfd;
	//是否非阻塞模式。
	bool m_IsNonBlock;
	//客户端连接的回调。
	std::function<void(struct ClientInfo &)> m_ClientConnectCallback;
};



#endif // !__SERVER_SOCKET_H__
