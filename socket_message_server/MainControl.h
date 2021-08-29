#ifndef __MAIN_CONTROL_H__
#define __MAIN_CONTROL_H__

#include <thread>

#include "SocketControl.h"
#include "DataHandler.h"


class MainControl
{
public:
	MainControl();
	~MainControl();

	bool Init();

	void Start();

	//启动监视。
	void StartMonitor();

private:
	//用户在线状态的回调。
	void UserOnlineStateCallback(int userId,bool state);
	//监视输出。
	void MonitorWork();


private:
	SocketControl * m_pSocketControl;
	DataHandler * m_pDataHandler;
	bool m_IsDestory;
	std::thread * m_pMonitorThread;
};

#endif // !__MAIN_CONTROL_H__
