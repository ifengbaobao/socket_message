#include <iostream>

#include "SocketMessageSdk.h"
#include "SocketMessageSdkDataType.h"
#include "CharsetUtils.h"

#include <thread>
#include <iostream>
#include <sstream>
#include <limits>



int main()
{
    SocketMessageSdk sdk;

	sdk.SetLoginCallback([](unsigned int id, unsigned int userId, std::string & userName) {
		std::cout << "登录结果：" << id << "用户id：" << userId << "用户名：" << userName.c_str() << std::endl;
	});
	sdk.SetUserListCallback([](unsigned int id, std::vector<UserInfo>& userVector) {
		std::cout << "所有的用户信息:" << "id:" << id << "用户个数:" << userVector.size() << std::endl << "----------" << std::endl;
		for (UserInfo & userItem : userVector)
		{
			std::cout << userItem.UserId << "    " << userItem.UserName.c_str() << std::endl;
		}
		std::cout << "----------" << std::endl;
	});

	sdk.SetGroupListCallback([&sdk](unsigned int id, std::vector<GroupInfo> & groupVector) {
		std::cout << "所有群组信息：" << "id:" << id << "群个数：" << groupVector.size() << std::endl << "----------" << std::endl;
		for (GroupInfo & groupItem : groupVector)
		{
			std::cout << groupItem.GroupId << "    " << groupItem.GroupName.c_str() << "    管理员id：" << groupItem.AdminId << "成员个数：" << groupItem.UserList.size() << std::endl;
			for (UserInfo & userItem : groupItem.UserList)
			{
				std::cout << "    " << userItem.UserId << "    " << userItem.UserName.c_str() << std::endl;
			}
		}
		std::cout << "----------" << std::endl;
		//获取离线消息

		std::cout << "获取离线消息:";
		bool b = sdk.SendGetDataMsgList(id);
		std::cout << b << std::endl;
	});

	sdk.SetDataMsgListCallback([](unsigned int id, std::vector<ChatMsg> & msgVector) {
		std::cout << "离线消息：" << id << std::endl << "----------" << std::endl;
		for (ChatMsg & msg : msgVector)
		{
			std::cout << std::endl;
			std::cout << "消息id：" << msg.MsgId << std::endl;
			std::cout << "消息类型：" << msg.MsgType << std::endl;
			std::cout << "发送者：" << msg.Sender << std::endl;
			std::cout << "接收者：" << msg.Receiver << std::endl;
			std::cout << "接收是否群组：" << msg.IsGroup << std::endl;
			std::cout << "发送消息的时间:" << msg.CreatedTime << std::endl;
#ifdef _WIN32
			std::string gMsg = CharsetUtils::Utf8ToGbk(msg.Msg);
			std::cout << "消息内容：" << gMsg.c_str() << std::endl;
#else
			std::cout << "消息内容：" << msg.Msg.c_str() << std::endl;
#endif

			std::cout << std::endl;
		}
		std::cout << "----------" << std::endl;
	});

	sdk.SetDataMsgCallback([](unsigned int id, ChatMsg & msg) {
		std::cout << "接收到实时消息：" << id << std::endl << "----------" << std::endl;
		std::cout << "消息id：" << msg.MsgId << std::endl;
		std::cout << "消息类型：" << msg.MsgType << std::endl;
		std::cout << "发送者：" << msg.Sender << std::endl;
		std::cout << "接收者：" << msg.Receiver << std::endl;
		std::cout << "接收是否群组：" << msg.IsGroup << std::endl;
		std::cout << "发送消息的时间:" << msg.CreatedTime << std::endl;
#ifdef _WIN32
		std::string gMsg = CharsetUtils::Utf8ToGbk(msg.Msg);
		std::cout << "消息内容：" << gMsg.c_str() << std::endl;
#else
		std::cout << "消息内容：" << msg.Msg.c_str() << std::endl;
#endif
		std::cout << "----------" << std::endl;
	});

	bool isConnect = false;
    unsigned int id = sdk.ConnectServer("192.168.0.104", 1234, isConnect);
    if(!isConnect)
    {
        std::cout << "连接服务器失败" << std::endl;
        return 0;
    }
	std::cout << "连接服务器结果：" << id << std::endl;


	std::cout << "准备登录：" << std::endl;

	std::cout << "输入用户id：";
	int userId;
	std::cin >> userId;

	std::cout << "输入密码：";
	char password[32] = { 0 };
	std::cin >> password;
	std::getchar();//多一个回车消化掉。
	std::cout << std::endl << "用户名：" << userId << "   密码：" << password << std::endl;

	bool b = sdk.SendLogin(id, userId, std::string(password));
	std::cout << "登录发送结果：" << b << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::cout << "开始聊天:" << std::endl;
	while (true)
	{
		int recv;
		std::cout << "输入接收者id：";
		std::cin >> recv;
		std::cout << "输入聊天内容：";
		char msg[1024] = { 0 };
		std::cin >> msg;

#ifdef _WIN32
		std::string gMsg(msg);
		std::string uMsg = CharsetUtils::GbkToUtf8(gMsg);
#else
		std::string uMsg(msg);
#endif
		b = sdk.SendDataMsg(id, MsgTypeText, recv, recv < 100000, uMsg);
		std::cout << "发送消息结果：" << b << std::endl;

		std::getchar();//消耗掉多余的回车。
	}

	std::getchar();
	return 0;
}

