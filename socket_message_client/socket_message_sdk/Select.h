#ifndef __SELECT_H__
#define __SELECT_H__

#include "SockeIncludeHead.h"
#include "ClientSocket.h"

#include <memory>
#include <map>
#include <thread>


class Select
{
public:
	Select();
	~Select();

	//添加
	void Add(int fs, std::shared_ptr<ClientSocket> cs);
	//删除
	void Remove(int fs);

	//客户端关闭的回调。
	void SetSocketCloseCallback(std::function<void(std::shared_ptr<ClientSocket>)> callback);

	void SetTimeout(int timeout = 100);

	void Start();

	std::shared_ptr<ClientSocket> GetClientSocket(SOCKET s);

private:
	void Work();




private:
	std::map<SOCKET, std::shared_ptr<ClientSocket> > m_ClientSocketMap;
	int m_Timeout;

	//客户端打开，和 客户端关闭的回调。
	std::function<void(std::shared_ptr<ClientSocket>)> m_SocketCloseCallback;

	//是否销毁。
	bool m_IsDestory;

	std::thread * m_pWorkThread;

};


#endif // !__SELECT_H__
