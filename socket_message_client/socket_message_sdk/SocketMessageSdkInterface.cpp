#include "SocketMessageSdkInterface.h"

#include "SocketControl.h"
#include "SockeIncludeHead.h"
#include "DataType.h"
#include "Utils.h"


const unsigned int SocketMessageSdkInterface::ID_INVALID = INVALID_SOCKET;

#define SEND_DATA(id, bean) \
	std::unique_ptr<char[]> data = bean.Serialize(); \
	int dataLength = bean.DataSize; \
	return m_pSocketControl->SendPackage(id, data.get(), dataLength);


SocketMessageSdkInterface::SocketMessageSdkInterface()
	:m_pSocketControl(new SocketControl())
	, m_SelfUserId(0)
{
	//socket断开的回调。
	m_pSocketControl->SetSocketCloseCallback([this](SOCKET s) {
		if (m_DisconnectServerCallback)
		{
			m_DisconnectServerCallback(s);
		}
	});

	//设置读的回调。
    m_pSocketControl->SetReadPackageCallback(std::bind(&SocketMessageSdkInterface::SocketReadReadPackageCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	RegeditDataHandler();
}

SocketMessageSdkInterface::~SocketMessageSdkInterface()
{
	delete m_pSocketControl;
}

unsigned int SocketMessageSdkInterface::ConnectServer(const char * ip, unsigned short port)
{
	SOCKET s;
	if (m_pSocketControl->ConnectServerSocket(ip, port, s))
	{
		return s;
	}
	return ID_INVALID;
}

void SocketMessageSdkInterface::SetDisconnectServerCallback(std::function<void(unsigned int)> callback)
{
	m_DisconnectServerCallback = callback;
}

bool SocketMessageSdkInterface::SendLogin(unsigned int id, int userId, std::string password)
{
	if (id == ID_INVALID)
	{
		return false;
	}
	C2SLogin login;
	login.UserId = userId;
	int passwordLength = sizeof(login.Password);
	if (passwordLength > (password.size()+1))
	{
		passwordLength = password.size()+1;
	}
	std::memcpy(login.Password, password.c_str(), passwordLength);

	SEND_DATA(id, login);
}

bool SocketMessageSdkInterface::SendDataMsg(unsigned int id, EMsgType msgType, int recvUserId, bool isGroup, std::string msg, std::string ext)
{
	if (id == ID_INVALID || m_SelfUserId == 0)
	{
		return false;
	}
	C2SDataMsg dataMsg;
	dataMsg.MsgType = msgType;
	dataMsg.Sender = m_SelfUserId;
	dataMsg.Receiver = recvUserId;
	dataMsg.IsGroup = isGroup;
	dataMsg.CreatedTime = Utils::GetTimeStampSeconds();
	dataMsg.Msg = msg;
	dataMsg.Extend = ext;

	SEND_DATA(id, dataMsg);
}

bool SocketMessageSdkInterface::SendGetDataMsgList(unsigned int id, int lastMsgId)
{
	if (id == ID_INVALID || m_SelfUserId == 0)
	{
		return false;
	}
	C2SGetDataMsgList getDataMsgList;
	getDataMsgList.UserId = m_SelfUserId;
	getDataMsgList.LastMsgId = lastMsgId;

	SEND_DATA(id, getDataMsgList);
}

void SocketMessageSdkInterface::SetLoginCallback(std::function<void(unsigned int, unsigned int, std::string&)> callback)
{
	m_LoginCallback = callback;
}

void SocketMessageSdkInterface::SetUserListCallback(std::function<void(unsigned int, std::vector<UserInfo>&)> callback)
{
	m_UserListCallback = callback;
}

void SocketMessageSdkInterface::SetGroupListCallback(std::function<void(unsigned int, std::vector<GroupInfo>&)> callback)
{
	m_GroupListCallback = callback;
}

void SocketMessageSdkInterface::SetDataMsgCallback(std::function<void(unsigned int, ChatMsg&)> callback)
{
	m_DataMsgCallback = callback;
}

void SocketMessageSdkInterface::SetDataMsgListCallback(std::function<void(unsigned int, std::vector<ChatMsg>&)> callback)
{
	m_DataMsgListCallback = callback;
}

void SocketMessageSdkInterface::SocketReadReadPackageCallback(SOCKET fd, std::unique_ptr<char[]>& data, int size)
{
	//读取服务器返回的数据。
	if (size < sizeof(DataHead))
	{
		return;
	}

	DataHead * dh = (DataHead *)data.get();
	if (dh->DataSize != size)
	{
		//说明数据长度不对。
		return;
	}
	EDataType dataType = dh->DataType;

	//std::map<EDataType, std::function<void(SOCKET, std::unique_ptr<char[]>, int)> > m_DataTypeMap;

	if (m_DataTypeMap.count(dataType) == 0)
	{
		//数据类型没有处理。
		return;
	}
	m_DataTypeMap.at(dataType)(fd,data,size);
}

#define DataTypeMapInsertData(name) \
	m_DataTypeMap.insert( \
		std::map< \
			EDataType, \
			std::function<void(SOCKET, std::unique_ptr<char[]>&, int)> \
		>::value_type( \
			EDataType_##name, \
			std::bind( \
                &SocketMessageSdkInterface::DataHandler##name, \
				this, \
				std::placeholders::_1, \
				std::placeholders::_2, \
				std::placeholders::_3 \
			) \
		) \
	);


void SocketMessageSdkInterface::RegeditDataHandler()
{
	DataTypeMapInsertData(S2CLogin);
	DataTypeMapInsertData(S2CUserList);
	DataTypeMapInsertData(S2CGroupList);
	DataTypeMapInsertData(S2CDataMsg);
	DataTypeMapInsertData(S2CDataMsgList);
}

void SocketMessageSdkInterface::DataHandlerS2CLogin(SOCKET fd, std::unique_ptr<char[]>& data, int size)
{
	if (m_LoginCallback)
	{
		S2CLogin login;
		login.UnSerialize(data.get());
		std::string userName(login.UserName);
		if (login.UserId != -1)
		{
			m_SelfUserId = login.UserId;
		}
		m_LoginCallback(fd, login.UserId,userName);
	}
}

void SocketMessageSdkInterface::DataHandlerS2CUserList(SOCKET fd, std::unique_ptr<char[]>& data, int size)
{
	if (m_UserListCallback)
	{
		std::vector<UserInfo> userVector;
		S2CUserList userList;
		userList.UnSerialize(data.get());
		for (S2CUser & userItem : userList.UserVector)
		{
			UserInfo ui;
			ui.UserId = userItem.UserId;
			ui.UserName.append(userItem.UserName);
			userVector.push_back(ui);
		}
		m_UserListCallback(fd, userVector);
	}
}

void SocketMessageSdkInterface::DataHandlerS2CGroupList(SOCKET fd, std::unique_ptr<char[]>& data, int size)
{
	if (m_GroupListCallback)
	{
		S2CGroupList gl;
		gl.UnSerialize(data.get());
		std::vector<GroupInfo> groupVector;
		for (S2CGroup & groupItem : gl.GroupVector)
		{
			GroupInfo gi;
			gi.GroupId = groupItem.GroupId;
			gi.GroupName = groupItem.GroupName;
			gi.AdminId = groupItem.AdminId;

			for (S2CUser & userItem : groupItem.UserList.UserVector)
			{
				UserInfo ui;
				ui.UserId = userItem.UserId;
				ui.UserName.append(userItem.UserName);
				gi.UserList.push_back(ui);
			}
			groupVector.push_back(gi);
		}
		m_GroupListCallback(fd, groupVector);
	}
}

void SocketMessageSdkInterface::DataHandlerS2CDataMsg(SOCKET fd, std::unique_ptr<char[]>& data, int size)
{
	if (m_DataMsgCallback)
	{
		S2CDataMsg dm;
		dm.UnSerialize(data.get());

		ChatMsg cm;
		cm.MsgId = dm.MsgId;
		cm.MsgType = dm.MsgType;
		cm.Sender = dm.Sender;
		cm.Receiver = dm.Receiver;
		cm.IsGroup = dm.IsGroup;
		cm.CreatedTime = dm.CreatedTime;
		cm.Msg = dm.Msg;
		cm.Extend = dm.Extend;
		
		m_DataMsgCallback(fd, cm);
	}
}

void SocketMessageSdkInterface::DataHandlerS2CDataMsgList(SOCKET fd, std::unique_ptr<char[]>& data, int size)
{
	if (m_DataMsgListCallback)
	{
		S2CDataMsgList msgList;
		msgList.UnSerialize(data.get());
		std::vector<ChatMsg> chatMsgVector;
		for (DataMsg msgItem : msgList.MsgVector)
		{
			ChatMsg cm;
			cm.MsgId = msgItem.MsgId;
			cm.MsgType = msgItem.MsgType;
			cm.Sender = msgItem.Sender;
			cm.Receiver = msgItem.Receiver;
			cm.IsGroup = msgItem.IsGroup;
			cm.CreatedTime = msgItem.CreatedTime;
			cm.Msg = msgItem.Msg;
			cm.Extend = msgItem.Extend;
			chatMsgVector.push_back(cm);
		}
		m_DataMsgListCallback(fd, chatMsgVector);
	}
}


//管理对象。
std::map<void *, std::shared_ptr<SocketMessageSdkInterface>> SocketMessageSdkInterface::s_ManageMap;

void SocketMessageSdkInterface::Add2Manage(void * key, std::shared_ptr<SocketMessageSdkInterface> val)
{
    s_ManageMap.insert(std::map<void *, std::shared_ptr<SocketMessageSdkInterface>>::value_type(key, val));
}

std::shared_ptr<SocketMessageSdkInterface> SocketMessageSdkInterface::Get2Manage(void * key)
{
	if (s_ManageMap.count(key) == 0)
	{
        return std::shared_ptr<SocketMessageSdkInterface>(nullptr);
	}
	else
	{
		return s_ManageMap.at(key);
	}
}

void SocketMessageSdkInterface::remove2Manage(void * key)
{
	s_ManageMap.erase(key);
}


