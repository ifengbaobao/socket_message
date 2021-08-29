#include "SocketMessageSdk.h"

#include "SocketMessageSdkInterface.h"

SocketMessageSdk::SocketMessageSdk()
{
    SocketMessageSdkInterface::Add2Manage(this, std::shared_ptr<SocketMessageSdkInterface>(new SocketMessageSdkInterface()));
}


SocketMessageSdk::~SocketMessageSdk()
{
    SocketMessageSdkInterface::remove2Manage(this);
}

unsigned int SocketMessageSdk::ConnectServer(const char * ip, unsigned short port, bool & isConnect)
{
	unsigned int id = SocketMessageSdkInterface::Get2Manage(this)->ConnectServer(ip,port);
	isConnect = (id != SocketMessageSdkInterface::ID_INVALID);
	return id;
}

void SocketMessageSdk::SetDisconnectServerCallback(std::function<void(unsigned int)> callback)
{
    SocketMessageSdkInterface::Get2Manage(this)->SetDisconnectServerCallback(callback);
}

bool SocketMessageSdk::SendLogin(unsigned int id, int userId, std::string password)
{
    return SocketMessageSdkInterface::Get2Manage(this)->SendLogin(id,userId,password);
}

bool SocketMessageSdk::SendDataMsg(unsigned int id, EMsgType msgType, int recvUserId, bool isGroup, std::string msg, std::string ext)
{
    return SocketMessageSdkInterface::Get2Manage(this)->SendDataMsg(id,msgType,recvUserId,isGroup,msg,ext);
}

bool SocketMessageSdk::SendGetDataMsgList(unsigned int id, int lastMsgId)
{
    return SocketMessageSdkInterface::Get2Manage(this)->SendGetDataMsgList(id,lastMsgId);
}

void SocketMessageSdk::SetLoginCallback(std::function<void(unsigned int, unsigned int, std::string&)> callback)
{
    SocketMessageSdkInterface::Get2Manage(this)->SetLoginCallback(callback);
}

void SocketMessageSdk::SetUserListCallback(std::function<void(unsigned int, std::vector<UserInfo>&)> callback)
{
    SocketMessageSdkInterface::Get2Manage(this)->SetUserListCallback(callback);
}

void SocketMessageSdk::SetGroupListCallback(std::function<void(unsigned int, std::vector<GroupInfo>&)> callback)
{
    SocketMessageSdkInterface::Get2Manage(this)->SetGroupListCallback(callback);
}

void SocketMessageSdk::SetDataMsgCallback(std::function<void(unsigned int, ChatMsg&)> callback)
{
    SocketMessageSdkInterface::Get2Manage(this)->SetDataMsgCallback(callback);
}

void SocketMessageSdk::SetDataMsgListCallback(std::function<void(unsigned int, std::vector<ChatMsg>&)> callback)
{
    SocketMessageSdkInterface::Get2Manage(this)->SetDataMsgListCallback(callback);
}
