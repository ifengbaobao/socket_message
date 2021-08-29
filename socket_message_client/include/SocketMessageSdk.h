#ifndef __SOCKET_MESSAGE_SDK_H__
#define __SOCKET_MESSAGE_SDK_H__

#ifdef _WIN32
#define SDKAPI _declspec(dllexport)
#else
#define SDKAPI
#endif

#include <functional>
#include <vector>

#include "SocketMessageSdkDataType.h"

class SDKAPI SocketMessageSdk
{
public:
    SocketMessageSdk();
    ~SocketMessageSdk();

	//--------------------------------------连接和断开-------------------------------------------------------
	//连接服务器。返回对应的id。参数，isConnect返回是否链接成功。
	unsigned int ConnectServer(const char * ip, unsigned short port, bool & isConnect);
	//设置断开连接的回调。
	void SetDisconnectServerCallback(std::function<void(unsigned int)> callback);

	//--------------------------------------发送消息。-----------------------------------------------------
	//登录。
	bool SendLogin(unsigned int id, int userId, std::string password);

	//实时聊天发送。文本消息。
	bool SendDataMsg(unsigned int id, EMsgType msgType, int recvUserId, bool isGroup, std::string msg, std::string ext = std::string(""));

	//接收离线消息。
	bool SendGetDataMsgList(unsigned int id, int lastMsgId = 0);


	//-------------------------------------接收消息-------------------------------------------------------

	//S2CLogin，用户登录的回调。
	void SetLoginCallback(std::function<void(unsigned int, unsigned int, std::string&)> callback);

	//S2CUserList，获取所有用户列表的回调。
	void SetUserListCallback(std::function<void(unsigned int, std::vector<UserInfo>&)> callback);

	//S2CGroupList，获取群组列表的回调。
	void SetGroupListCallback(std::function<void(unsigned int, std::vector<GroupInfo>&)> callback);

	//S2CDataMsg,接收到实时消息的回调。
	void SetDataMsgCallback(std::function<void(unsigned int, ChatMsg&)> callback);

	//S2CDataMsgList，接收到离线消息的回调。
	void SetDataMsgListCallback(std::function<void(unsigned int, std::vector<ChatMsg>&)> callback);

};



#endif // !__SOCKET_MESSAGE_SDK_H__
