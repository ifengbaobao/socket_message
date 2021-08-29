
#include <iostream>

#include "MemoryHook.h"
#include "MainControl.h"

#include "MySql.h"

#include "Config.h"
#include "DBUser.h"
#include "DataType.h"


int main()
{
	MainControl mc;
	if (mc.Init())
	{
		std::cout << "初始化成功！" << std::endl;
	}
	else
	{
		std::cout << "初始化失败！" << std::endl;
	}
	mc.Start();
	std::cout << "启动！" << std::endl;
	mc.StartMonitor();
	std::cout << "监控启动！" << std::endl;



	std::getchar();
	return 0;
}
