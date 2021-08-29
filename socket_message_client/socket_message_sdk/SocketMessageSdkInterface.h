#ifndef __SOCKET_MESSAGE_SDK_INTERFACE_H__
#define __SOCKET_MESSAGE_SDK_INTERFACE_H__

#include "SocketMessageSdkDataType.h"
#include "SocketControl.h"
#include "DataType.h"

#include <map>


class SocketMessageSdkInterface
{
public:
    SocketMessageSdkInterface();
    ~SocketMessageSdkInterface();
	
	//--------------------------------------连接和断开-------------------------------------------------------
	//连接服务器。返回对应的id。
	unsigned int ConnectServer(const char * ip, unsigned short port);
	//设置断开连接的回调。
	void SetDisconnectServerCallback(std::function<void(unsigned int)> callback);

	//--------------------------------------发送消息。-----------------------------------------------------
	//登录。
	bool SendLogin(unsigned int id, int userId,std::string password);

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

private:
	//读取服务器返回回来的数据。
	void SocketReadReadPackageCallback(SOCKET fd, std::unique_ptr<char[]>& data, int size);

	//注册消息类型。
	void RegeditDataHandler();

	void DataHandlerS2CLogin(SOCKET fd, std::unique_ptr<char[]>& data, int size);
	void DataHandlerS2CUserList(SOCKET fd, std::unique_ptr<char[]>& data, int size);
	void DataHandlerS2CGroupList(SOCKET fd, std::unique_ptr<char[]>& data, int size);
	void DataHandlerS2CDataMsg(SOCKET fd, std::unique_ptr<char[]>& data, int size);
	void DataHandlerS2CDataMsgList(SOCKET fd, std::unique_ptr<char[]>& data, int size);

private:
	SocketControl * m_pSocketControl;
	//断开连接的回调。
	std::function<void(unsigned int)> m_DisconnectServerCallback;
	//用户id。
	int m_SelfUserId;

	//登录的回调。第一个参数，id(socket id) 第二个参数 用户id，第三个参数，用户名。
	std::function<void(unsigned int, unsigned int, std::string&)> m_LoginCallback;

	//获取所有的用户信息。
	std::function<void(unsigned int, std::vector<UserInfo>&)> m_UserListCallback;

	//获取所有的群的信息。
	std::function<void(unsigned int, std::vector<GroupInfo>&)> m_GroupListCallback;

	//接收实时聊天消息。
	std::function<void(unsigned int, ChatMsg&)> m_DataMsgCallback;

	//接收离线的消息。
	std::function<void(unsigned int, std::vector<ChatMsg>&)> m_DataMsgListCallback;


	//消息类型的处理。
	std::map<EDataType, std::function<void(SOCKET, std::unique_ptr<char[]>&, int)> > m_DataTypeMap;

public:
	static const unsigned int ID_INVALID;

	//对象管理。
private:
    static std::map<void *, std::shared_ptr<SocketMessageSdkInterface>> s_ManageMap;
public:
    static void Add2Manage(void * key, std::shared_ptr<SocketMessageSdkInterface> val);
    static std::shared_ptr<SocketMessageSdkInterface> Get2Manage(void* key);
	static void remove2Manage(void * key);
};



#endif // !__SOCKET_MESSAGE_SDK_INTERFACE_H__
