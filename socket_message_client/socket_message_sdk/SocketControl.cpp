#include "SocketControl.h"

#include "SocketMessageSdkConfig.h"
#include "SocketHead.h"


SocketControl::SocketControl()
    :m_SocketConnectTimeout(CONFIG_SOCKET_CONNECT_TIMEOUT)
    , m_SocketSelectTimeout(CONFIG_SOCKET_SELECT_TIMEOUT)
	, m_IsNonBlock(true)
{
	m_Select.SetTimeout(m_SocketSelectTimeout);
	m_Select.SetSocketCloseCallback([this](std::shared_ptr<ClientSocket> sp) {
		if (m_SocketClostCallback)
		{
			m_SocketClostCallback(sp->GetFd());
		}
	});
}


SocketControl::~SocketControl()
{
}

bool SocketControl::ConnectServerSocket(const char * ip, unsigned short port,SOCKET & newFd)
{
	ClientSocket * cs = new ClientSocket();
	cs->SetNonBlock(m_IsNonBlock);
	if (!cs->Connect(ip, port, m_SocketConnectTimeout))
	{
		delete cs;
		newFd = INVALID_SOCKET;
		return false;
	}
	cs->SetReadDataCallback(std::bind(&SocketControl::ClientReadPackageCallback, this,std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	cs->SetWriteDataCallback([](ClientSocket*cs, bool hasData) {
		cs->GetCfdIsWrite() = hasData;
	});
	m_Select.Add(cs->GetFd(), std::shared_ptr<ClientSocket>(cs));
	m_Select.Start();
	newFd = cs->GetFd();
	return true;
}

bool SocketControl::SendPackage(SOCKET fd, char * data, int size)
{
	std::shared_ptr<ClientSocket> sp = m_Select.GetClientSocket(fd);
	if (!sp)
	{
		return false;
	}
	int emptySize = sp->GetSize4EmptyWrite();
	if (emptySize < size + SocketHead::SocketHeadSize)
	{
		return false;
	}

	int sendDataCount = SocketHead::SocketHeadSize + size;
	SocketHead head = {};
	head.PackageCount = size;

	char * sendData = new char[sendDataCount];
	head.Serialize(sendData);
	memcpy(sendData + SocketHead::SocketHeadSize, data, size);
	bool ret = true;
	do
	{
		if (ClientSocket::SOCKET_CLOSE == sp->Write(sendData, sendDataCount))
		{
			m_Select.Remove(fd);
			ret = false;
			break;
		}
	} while (false);
	delete[] sendData;

	//添加socket的头。
	return ret;
}

void SocketControl::SetReadPackageCallback(std::function<void(SOCKET, std::unique_ptr<char[]>&, int)> callback)
{
	m_ReadPackageCallback = callback;
}

void SocketControl::SetSocketCloseCallback(std::function<void(SOCKET)> callback)
{
	m_SocketClostCallback = callback;
}

int SocketControl::ClientReadPackageCallback(ClientSocket * cs, char * data, int size)
{
	int offset = 0;
	SocketHead head;
	while (size > SocketHead::SocketHeadSize + offset) {
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
			m_ReadPackageCallback(cs->GetFd(), up, packageCount);
		}
		offset += (SocketHead::SocketHeadSize + packageCount);
	};
	return offset;
}
