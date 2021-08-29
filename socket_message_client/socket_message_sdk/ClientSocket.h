#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

#include <cstring>
#include <functional>
#include <mutex>
#include <atomic>

#include "SockeIncludeHead.h"


struct ClientInfo
{
	SOCKET cfd;
	unsigned short port;
	char ip[16];
	ClientInfo()
	{
		cfd = INVALID_SOCKET;
		port = 0;
		memset(ip, 0, sizeof(ip));
	}
};

class ClientSocket
{
public:
	ClientSocket(struct ClientInfo && clientInfo = ClientInfo());
	~ClientSocket();

	//接收数据。
	int Recv();

	//写数据。
	int Write(char * pData,int size);

	//发送数据。
	int Send();

	//读取数据的回调。返回处理完的数据的个数，第一个参数数据指针，第二个参数数据大小。
	void SetReadDataCallback(std::function<int(ClientSocket*,char *, int)> callback);

	//客户端写数据的回调。参数true：非阻塞模式，数据还有，需要监听再次处理。参数false：非阻塞模式，数据处理完成了。
	void SetWriteDataCallback(std::function<void(ClientSocket*, bool)> callback);

	//获取文件描述符。
	SOCKET GetFd();
	//获取客户端端口。
	unsigned short GetPort();
	//获取客户端ip
	char* GetIp();

	//获取可写的空间的大小
	int GetSize4EmptyWrite();

	//关于cfd的读写
	std::atomic_bool& GetCfdIsRead();
	std::atomic_bool& GetCfdIsWrite();


	//设置非阻塞模式。
	bool SetNonBlock(bool isNonBlock);

	//创建socket。
	bool CreateSocket();

	//超时时间 timeout 毫秒。
	bool Connect(const char * addr, unsigned short port, int timeout = -1);

private:
	//设置非阻塞模式。
	bool  SetNonBlockOperation(bool isNonBlock);

private:
	struct ClientInfo m_ClientInfo;
	//是否非阻塞模式。
	bool m_IsNonBlock;

	//读取数据的缓存大小。
	char * m_ReadBuf;
	//当前读缓存的内容个数。
	int m_CurReadBufSize;
	//读缓存的最大大小。
	int m_ReadBufMaxSize;

	//写数据的缓存大小。
	char * m_WriteBuf;
	//写缓存的大小。
	int m_CurWriteBufSize;
	//写缓存的最大大小。
	int m_WriteBufMaxSize;

	//客户端读取数据的回调。
	std::function<int(ClientSocket*,char *, int)> m_ReadDataCallback;
	//客户端写数据的回调。参数true：非阻塞模式，数据还有，需要监听再次处理。参数false：非阻塞模式，数据处理完成了。
	std::function<void(ClientSocket*, bool)> m_WriteDataCallback;

	//是否正在写。
	bool m_IsWriteing;

	std::mutex m_MutexRead;
	std::mutex m_MutexWrite;

	//关于cfd的读写
	std::atomic_bool m_CfdIsRead;
	std::atomic_bool m_CfdIsWrite;

#ifdef _WIN32
	static std::atomic_int s_ClientSocketCount;
#endif

public:
	//socket 关闭。
	static const int SOCKET_CLOSE;

};


#endif // !__CLIENT_SOCKET_H__
