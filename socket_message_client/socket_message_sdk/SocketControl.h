#ifndef __SOCKET_CONTROL_H__
#define __SOCKET_CONTROL_H__

#include "ClientSocket.h"
#include "Select.h"

class SocketControl
{
public:
	SocketControl();
	~SocketControl();

	//socket 连接。连接服务器。可以连接多个服务器。
	bool ConnectServerSocket(const char * ip, unsigned short port, SOCKET & newFd);

	//socket断开的回调。
	void SetSocketCloseCallback(std::function<void(SOCKET)> callback);

	//写数据。
	bool SendPackage(SOCKET fd, char * data, int size);

	//设置读的回调。
	void SetReadPackageCallback(std::function<void(SOCKET, std::unique_ptr<char[]>&, int)> callback);


private:
	int ClientReadPackageCallback(ClientSocket* cs, char* data, int size);

private:
	int m_SocketConnectTimeout;
	int m_SocketSelectTimeout;
	Select m_Select;
	bool m_IsNonBlock;

	std::function<void(SOCKET, std::unique_ptr<char[]>&, int)> m_ReadPackageCallback;

	std::function<void(SOCKET)> m_SocketClostCallback;


};


#endif // !__SOCKET_CONTROL_H__
