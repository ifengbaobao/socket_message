#include "MainControl.h"

#include <iostream>

#include "Config.h"


MainControl::MainControl()
    :m_pSocketControl(new SocketControl(CONFIG_SOCKET_THREAD_COUNT))
    , m_pDataHandler(new DataHandler(CONFIG_DATA_HANDLER_THREAD_COUNT, CONFIG_MYSQL_CONNECT_COUNT))
	, m_pMonitorThread(nullptr)
	, m_IsDestory(false)
{
	//发送数据的回调。
	m_pDataHandler->SetSendPackageCallback(std::bind(&SocketControl::SendPackage
		, m_pSocketControl
		, std::placeholders::_1
		, std::placeholders::_2
		, std::placeholders::_3
	));

	//用户在线状态变化的回调，上线下线的回调。
	m_pDataHandler->SetUserOnlineStateCallback(std::bind(&MainControl::UserOnlineStateCallback
		,this
		, std::placeholders::_1
		, std::placeholders::_2));

	//客户端socket关闭的回调
	//std::function<void(int)> callback
	m_pDataHandler->SetCloseClientSocketCallback(std::bind(
		&SocketControl::ClientSocketClose
		, m_pSocketControl
		, std::placeholders::_1
	));

	//读到数据包的回调。
	m_pSocketControl->SetReadPackageCallback(std::bind(
		&DataHandler::AddData
		, m_pDataHandler
		, std::placeholders::_1
		, std::placeholders::_2
		, std::placeholders::_3
	));

	//客户端打开和关闭的回调。
	//std::function<void(int, bool)> callback
	m_pSocketControl->SetClientSocketStateCallback(std::bind(&DataHandler::ClientSocketState
		, m_pDataHandler
		, std::placeholders::_1
		, std::placeholders::_2
	));

}


MainControl::~MainControl()
{
	m_IsDestory = true;
	if (m_pMonitorThread != nullptr)
	{
		m_pMonitorThread->join();
		delete m_pMonitorThread;
		m_pMonitorThread = nullptr;
	}
	delete m_pDataHandler;
	m_pDataHandler = nullptr;
	delete m_pSocketControl;
	m_pSocketControl = nullptr;

}

bool MainControl::Init()
{
    if (!m_pDataHandler->InitMySql(CONFIG_MYSQL_SERVER, CONFIG_MYSQL_USER, CONFIG_MYSQL_PASSWORD, CONFIG_MYSQL_DATABASE, CONFIG_MYSQL_PORT))
	{
		return false;
	}
    if (!m_pSocketControl->Init(CONFIG_SERVICE_SOCKET_PORT, CONFIG_EPOLL_TIMEOUT))
	{
		return false;
	}
	return true;
}

void MainControl::Start()
{
	m_pSocketControl->Start();
}

void MainControl::StartMonitor()
{
	if (m_pMonitorThread == nullptr)
	{
		m_pMonitorThread = new std::thread(&MainControl::MonitorWork,this);
	}
}

void MainControl::UserOnlineStateCallback(int userId, bool state)
{
	std::cout << "用户：" << userId << (state ? "上线" : "下线") << std::endl;
}

void MainControl::MonitorWork()
{
	char* outStr = new char[512];
	while (!m_IsDestory)
	{
		std::sprintf(outStr, "数据处理：用户数：%5d，空线程数：%2d，可用的sql连接：%2d，任务队列数：%4d，socket数据：空线程数：%2d，任务数：%4d,连接数：%5d，发送数据：%6d,%8d,接收数据：%6d,%8d"
			, m_pDataHandler->GetOnLineUserCount()
			, m_pDataHandler->GetAvailableThreadCount()
			, m_pDataHandler->GetAvailableMySqlCount()
			, m_pDataHandler->GetQueueTaskCount()
			, m_pSocketControl->GetAvailableThreadCount()
			, m_pSocketControl->GetQueueTaskCount()
			, m_pSocketControl->GetClientSocketCount()
			, m_pSocketControl->GetSendPackageCount()
			, m_pSocketControl->GetSendByteCount()
			, m_pSocketControl->GetRecvPackageCount()
			, m_pSocketControl->GetRecvByteCount()
		);
		m_pSocketControl->ClearSendRecvCount();
		std::cout << outStr << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	delete[] outStr;
}
