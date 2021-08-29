#ifndef __SOCKET_CONTROL_H__
#define __SOCKET_CONTROL_H__

#include <map>
#include <memory>
#include <functional>
#include <atomic>

#include "EPoll.h"
#include "ServerSocket.h"
#include "ThreadPool.h"

class SocketControl
{
public:
	SocketControl(int threadCount = 4);
	~SocketControl();

	//epoll每次等待的超时时间，单位毫秒。
	bool Init(unsigned short port,int epollTimeout = -1);

	//开始。
	void Start();

	//读到数据包的回调。
	//第一个参数:调用者
	//第二个参数：文件描述符，可以作为客户端id。
	//第三个参数：数据。
	//第四个参数：数据大小。
	void SetReadPackageCallback(std::function<void(int, std::unique_ptr<char[]>&, int)> callback);

	//发送数据。
	//第一个参数：客户端文件描述符，可以作为客户端id。
	//第二个参数：发送的数据。
	//第三个参数：发送数据的大小。
	bool SendPackage(int cfd,char * data, int size);

	//客户端打开和关闭的回调。
	void SetClientSocketStateCallback(std::function<void(int,bool)> callback);

	//客户端关闭。
	void ClientSocketClose(int cfd);

	//关于发送接收数据的统计
	//清空发送接收的数据个数。
	void ClearSendRecvCount();
	unsigned long long GetSendPackageCount();
	unsigned long long GetSendByteCount();
	unsigned long long GetRecvPackageCount();
	unsigned long long GetRecvByteCount();

	//获取空线程的个数。可用线程的个数。
	int GetAvailableThreadCount();
	//获取任务队列中等待执行的任务数。
	int GetQueueTaskCount();

	//获取客户端socket的连接个数。
	int GetClientSocketCount();

private:
	void Server4ClientConnectCallback(struct ClientInfo & clientInfo);
	void Epoll4EventCallback(void* data, bool isRead, bool isWrite);

	//Epoll循环的线程。
	void EpollWork();

	//客户端的
	void ClientSocketCloseAndDelete(ClientSocket* clientSocket);
	int Client4ReadCallback(ClientSocket* clientSocket, char * data, int size);
	//客户端写数据的回调。参数true：非阻塞模式，数据还有，需要监听再次处理。参数false：非阻塞模式，数据处理完成了。
	void Client4WriteCallback(ClientSocket* clientSocket, bool hasData);

	ClientSocket * GetClientSocket(int cfd);

	//epoll上添加和删除客户端的读写。
	void EPollAddClientRead(ClientSocket * clientSocket);
	void EPollAddClientWrite(ClientSocket * clientSocket);
	void EPollDelClientRead(ClientSocket * clientSocket);
	void EPollDelClientWrite(ClientSocket * clientSocket);
	void EPollDelClient(ClientSocket * clientSocket);

private:
	EPoll * m_pEPoll;
	ServerSocket * m_pServerSocket;
	//是否非阻塞模式。
	bool m_IsNonBlock;

	//是否销毁。
	bool m_IsDestory;

	ThreadPool * m_pThreadPool;

	//epoll每次等待的超时时间，单位毫秒。
	int m_EpollTimeout;

	//客户端文件描述符的集合。
	std::map<int,ClientSocket*> m_ClientFdMap;
	//客户端fd的锁。
	std::mutex m_ClientFdMutex;
	//epoll添加删除数据的锁。
	std::mutex m_EPollMutex;

	//第一个参数：文件描述符，可以作为客户端id。
	//第二个参数：数据。
	//第三个参数：数据大小。
	std::function<void(int,std::unique_ptr<char[]>&,int)> m_ReadPackageCallback;

	//客户端打开，和 客户端关闭的回调。
	std::function<void(int,bool)> m_ClientSocketStateCallback;

	//是否初始化。
	bool m_IsInit;

	//数据统计。
	std::atomic_ullong m_SendPackageCount;
	std::atomic_ullong m_SendByteCount;
	std::atomic_ullong m_RecvPackageCount;
	std::atomic_ullong m_RecvByteCount;

	//客户端socket的个数，一共有多少个客户端连接。
	std::atomic_int m_ClientSocketCount;

};


#endif // !__SOCKET_CONTROL_H__
