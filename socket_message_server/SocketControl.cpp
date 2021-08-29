#include "SocketControl.h"

#include <functional>
#include <iostream>

#include "SocketHead.h"


SocketControl::SocketControl(int threadCount)
	: m_IsDestory(false)
	, m_IsNonBlock(true)
	, m_EpollTimeout(-1)
	, m_pEPoll(new EPoll())
	, m_pServerSocket(new ServerSocket())
	, m_pThreadPool(new ThreadPool(threadCount))
	, m_IsInit(false)
	, m_ClientSocketCount(0)
{
	//客户端有新连接的回调
	m_pServerSocket->SetClientConnectCallback(std::bind(&SocketControl::Server4ClientConnectCallback, this, std::placeholders::_1));
	//有消息的回调。
	m_pEPoll->SetEventCallback(std::bind(&SocketControl::Epoll4EventCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}


SocketControl::~SocketControl()
{
	m_IsDestory = true;
	{
		std::lock_guard<std::mutex> lock(m_ClientFdMutex);
		for (auto  begin = m_ClientFdMap.begin(); begin != m_ClientFdMap.end(); ++begin)
		{
			m_pEPoll->Remove(begin->first);
			delete begin->second;
		}
		m_ClientFdMap.clear();
	}
	delete m_pEPoll;
	delete m_pServerSocket;
	delete m_pThreadPool;
}


bool SocketControl::Init(unsigned short port, int epollTimeout)
{
	if (m_IsInit)
	{
		return true;
	}
	if (!m_pServerSocket->Init(port))
	{
		return false;
	}
	if (m_IsNonBlock)
	{
		m_pServerSocket->SetNonBlock();
	}
	//服务器socket挂到epoll
	m_pEPoll->Add(m_pServerSocket->GetFd(), m_pServerSocket);
	m_EpollTimeout = epollTimeout;
	m_IsInit = true;
	return true;
}

void SocketControl::Start()
{
	if (m_IsInit)
	{
		//epoll 的任务添加线程池中。
		m_pThreadPool->AddTask(std::bind(&SocketControl::EpollWork, this));
	}
}

void SocketControl::SetReadPackageCallback(std::function<void(int, std::unique_ptr<char[]>&, int)> callback)
{
	m_ReadPackageCallback = callback;
}

bool SocketControl::SendPackage(int cfd, char * data, int size)
{
	ClientSocket * cs = GetClientSocket(cfd);
	if (cs == nullptr)
	{
		return false;
	}
	//客户端的缓存已经满了。
	if (cs->GetSize4EmptyWrite() < size + SocketHead::SocketHeadSize)
	{
		return false;
	}
	int sendDataCount = SocketHead::SocketHeadSize + size;

	SocketHead head = {};
	head.PackageCount = size;
	
	char * sendData = new char[sendDataCount];
	head.Serialize(sendData);
	memcpy(sendData + SocketHead::SocketHeadSize, data, size);
	m_pThreadPool->AddTask([this,cfd, sendData, sendDataCount] {
		ClientSocket * cs = GetClientSocket(cfd);
		if (cs)
		{
			if (ClientSocket::SOCKET_CLOSE == cs->Write(sendData, sendDataCount))
			{
				ClientSocketCloseAndDelete(cs);
			}

			//发送数据的统计
			++m_SendPackageCount;
			m_SendByteCount += sendDataCount;
		}
		delete[] sendData;
	});
}

void SocketControl::SetClientSocketStateCallback(std::function<void(int,bool)> callback)
{
	m_ClientSocketStateCallback = callback;
}

void SocketControl::ClientSocketClose(int cfd)
{
	ClientSocket * cs = GetClientSocket(cfd);
	if (cs == nullptr)
	{
		return;
	}
	ClientSocketCloseAndDelete(cs);
}

void SocketControl::ClearSendRecvCount()
{
	//数据统计。
	m_SendPackageCount = 0;
	m_SendByteCount = 0;
	m_RecvPackageCount = 0;
	m_RecvByteCount = 0;
}

unsigned long long SocketControl::GetSendPackageCount()
{
	return m_SendPackageCount;
}

unsigned long long SocketControl::GetSendByteCount()
{
	return m_SendByteCount;
}

unsigned long long SocketControl::GetRecvPackageCount()
{
	return m_RecvPackageCount;
}

unsigned long long SocketControl::GetRecvByteCount()
{
	return m_RecvByteCount;
}

int SocketControl::GetAvailableThreadCount()
{
	return m_pThreadPool->GetAvailableThreadCount();
}

int SocketControl::GetQueueTaskCount()
{
	return m_pThreadPool->GetTaskCount();
}

void SocketControl::Server4ClientConnectCallback(ClientInfo & clientInfo)
{
	ClientSocket * clientSocket = new ClientSocket(clientInfo);
	if (m_IsNonBlock)
	{
		clientSocket->SetNonBlock();
	}
	clientSocket->SetReadDataCallback(std::bind(&SocketControl::Client4ReadCallback,this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	clientSocket->SetWriteDataCallback(std::bind(&SocketControl::Client4WriteCallback, this, std::placeholders::_1, std::placeholders::_2));
	{
		std::lock_guard<std::mutex> lock(m_ClientFdMutex);
		m_ClientFdMap.insert(std::map<int,ClientSocket*>::value_type(clientSocket->GetFd(),clientSocket));
	}
	++m_ClientSocketCount;
	if (m_ClientSocketStateCallback)
	{
		m_ClientSocketStateCallback(clientSocket->GetFd(),true);
	}
	EPollAddClientRead(clientSocket);
}

void SocketControl::Epoll4EventCallback(void * data, bool isRead, bool isWrite)
{
	//有新的客户端连接。
	if (data == m_pServerSocket)
	{
		{
			std::unique_lock<std::mutex> lock(m_EPollMutex);
			m_pEPoll->Remove(m_pServerSocket->GetFd());
		}
		m_pThreadPool->AddTask([this](){
			m_pServerSocket->Accept();
			//如果epoll需要重新添加，就在这儿添加。
			std::unique_lock<std::mutex> lock(m_EPollMutex);
			m_pEPoll->Add(m_pServerSocket->GetFd(), m_pServerSocket);
		});
		return;
	}
	//客户端有新数据要处理。
	ClientSocket * clientSocket = (ClientSocket *)data;
	if(isRead)
	{
		EPollDelClientRead(clientSocket);
		m_pThreadPool->AddTask([this, clientSocket]{
			if (ClientSocket::SOCKET_CLOSE == clientSocket->Recv())
			{
				ClientSocketCloseAndDelete(clientSocket);
			}
			else
			{
				EPollAddClientRead(clientSocket);
			}
		});
	}
	else if (isWrite)
	{
		EPollDelClientWrite(clientSocket);
		m_pThreadPool->AddTask([this, clientSocket]{
			if (ClientSocket::SOCKET_CLOSE == clientSocket->Send())
			{
				ClientSocketCloseAndDelete(clientSocket);
			}
			else
			{
				EPollAddClientWrite(clientSocket);
			}
		});
	}
}

void SocketControl::EpollWork()
{
	while (!m_IsDestory)
	{
		m_pEPoll->Wait(m_EpollTimeout);
	}
}

//客户端的关闭。
void SocketControl::ClientSocketCloseAndDelete(ClientSocket* clientSocket)
{
	//map中删除客户端。
	{
		std::lock_guard<std::mutex> lock(m_ClientFdMutex);
		if (0 == m_ClientFdMap.erase(clientSocket->GetFd()))
		{
			return;
		}
	}
	//epoll树上删除。
	EPollDelClient(clientSocket);
	m_ClientSocketCount--;
	if (m_ClientSocketStateCallback)
	{
		m_ClientSocketStateCallback(clientSocket->GetFd(),false);
	}
	delete clientSocket;
}

//客户端写数据的回调。参数true：非阻塞模式，数据还有，需要监听再次处理。参数false：非阻塞模式，数据处理完成了。
void SocketControl::Client4WriteCallback(ClientSocket* clientSocket, bool hasData)
{
	if (hasData)
	{
		EPollAddClientWrite(clientSocket);
	}
	else
	{
		EPollDelClientWrite(clientSocket);
	}
}

ClientSocket * SocketControl::GetClientSocket(int cfd)
{
	std::lock_guard<std::mutex> lock(m_ClientFdMutex);
	//客户端已经关闭了。
	if (m_ClientFdMap.count(cfd) == 0)
	{
		return nullptr;
	}
	return m_ClientFdMap.at(cfd);
}

void SocketControl::EPollAddClientRead(ClientSocket * clientSocket)
{
	std::unique_lock<std::mutex> lock(m_EPollMutex);
	if ( clientSocket->GetCfdIsRead())
	{
		return;
	}
	clientSocket->GetCfdIsRead() = true;
	if (clientSocket->GetCfdIsWrite())
	{
		m_pEPoll->Change(clientSocket->GetFd(), clientSocket, true);
	}
	else
	{
		m_pEPoll->Add(clientSocket->GetFd(), clientSocket);
	}
}

void SocketControl::EPollAddClientWrite(ClientSocket * clientSocket)
{
	std::unique_lock<std::mutex> lock(m_EPollMutex);
	if (clientSocket->GetCfdIsWrite())
	{
		return;
	}
	clientSocket->GetCfdIsWrite() = true;
	if (clientSocket->GetCfdIsRead())
	{
		m_pEPoll->Change(clientSocket->GetFd(), clientSocket, true);
	}
	else
	{
		m_pEPoll->Add(clientSocket->GetFd(), clientSocket,true,false);
	}
}

void SocketControl::EPollDelClientRead(ClientSocket * clientSocket)
{
	std::unique_lock<std::mutex> lock(m_EPollMutex);
	if (!clientSocket->GetCfdIsRead())
	{
		return;
	}
	clientSocket->GetCfdIsRead() = false;
	if (clientSocket->GetCfdIsWrite())
	{
		m_pEPoll->Change(clientSocket->GetFd(), clientSocket, true,false);
	}
	else
	{
		m_pEPoll->Remove(clientSocket->GetFd());
	}
}

void SocketControl::EPollDelClientWrite(ClientSocket * clientSocket)
{
	std::unique_lock<std::mutex> lock(m_EPollMutex);
	if (!clientSocket->GetCfdIsWrite())
	{
		return;
	}
	clientSocket->GetCfdIsWrite() = false;
	if (clientSocket->GetCfdIsRead())
	{
		m_pEPoll->Change(clientSocket->GetFd(), clientSocket);
	}
	else
	{
		m_pEPoll->Remove(clientSocket->GetFd());
	}
}

void SocketControl::EPollDelClient(ClientSocket * clientSocket)
{
	std::unique_lock<std::mutex> lock(m_EPollMutex);
	clientSocket->GetCfdIsWrite() = false;
	clientSocket->GetCfdIsRead() = false;
	m_pEPoll->Remove(clientSocket->GetFd());
}

int SocketControl::GetClientSocketCount()
{
	return m_ClientSocketCount;
}

int SocketControl::Client4ReadCallback(ClientSocket* clientSocket, char * data, int size)
{
	int offset = 0;
	SocketHead head;
	while(size > SocketHead::SocketHeadSize + offset) {
		head.UnSerialize(data + offset);
		int packageCount = head.PackageCount;
		if (size < offset + SocketHead::SocketHeadSize + packageCount)
		{
			break;
		}
		char * package = new char[packageCount];
		memcpy(package, data + SocketHead::SocketHeadSize + offset, packageCount);
		std::unique_ptr<char[]> up(package);
		if (m_ReadPackageCallback)
		{
			m_ReadPackageCallback(clientSocket->GetFd(), up, packageCount);
		}
		offset += (SocketHead::SocketHeadSize + packageCount);
		//接收的数据的统计。
		++m_RecvPackageCount;
		m_RecvByteCount += SocketHead::SocketHeadSize;
		m_RecvByteCount += packageCount;
	};
	return offset;
}


