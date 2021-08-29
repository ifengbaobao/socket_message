#include "DataHandler.h"
#include "DBUser.h"
#include "DBGroup.h"
#include "DBMsg.h"
#include "Utils.h"


DataHandler::DataHandler(int threadCount,int mySqlPoolCount)
	: m_pThreadPool(new ThreadPool(threadCount)), m_pMySqlPool(new MySqlPool(mySqlPoolCount))
{

	RegeditDataHandler();
}

DataHandler::~DataHandler()
{
	delete m_pThreadPool;
}

bool DataHandler::InitMySql(char * server, char * user, char * password, char * database, int port)
{
	return m_pMySqlPool->Init(server,user,password,database,port);
}

void DataHandler::AddData(int cfd, std::unique_ptr<char[]>& data, int size)
{
	std::shared_ptr<DataPack> sp(new DataPack);
	sp->cfd = cfd;
	sp->pData = data.release();
	sp->size = size;
	m_pThreadPool->AddTask(std::bind(&DataHandler::ThreadWork,this,sp));
}

void DataHandler::ThreadWork(std::shared_ptr<DataPack>& sp)
{
	if (sp->size < sizeof(DataHead))
	{
		return;
	}

	DataHead * dh = (DataHead *)sp->pData;
	if (dh->DataSize != sp->size)
	{
		//说明数据长度不对。
		return;
	}
	EDataType dataType = dh->DataType;

	if (m_DataTypeMap.count(dataType) == 0)
	{
		//数据类型没有处理。
		return;
	}
	m_DataTypeMap.at(dataType)(sp);
}

void DataHandler::ClientOpen(int cfd, int userId)
{
	//用户上线
	std::unique_lock<std::mutex> lock(m_OnlineUserMutex);
	m_OnlineUserMap.insert(std::map<int, int>::value_type(userId, cfd));
}

void DataHandler::ClientSocketState(int cfd, bool state)
{
	if (state)
	{
		//说明有新的socket连接。
		return;
	}
	std::unique_lock<std::mutex> lock(m_OnlineUserMutex);
	for (std::map<int, int>::iterator begin = m_OnlineUserMap.begin(); begin != m_OnlineUserMap.end(); ++begin)
	{
		if (begin->second == cfd)
		{
			int userId = begin->first;
			m_OnlineUserMap.erase(begin);
			break;
		}
	}
}

void DataHandler::SetSendPackageCallback(std::function<void(int, char*, int)> callback)
{
	m_SendPackageCallback = callback;
}

void DataHandler::SetUserOnlineStateCallback(std::function<void(int, bool)> callback)
{
	m_UserOnlineStateCallback = callback;
}

void DataHandler::SetCloseClientSocketCallback(std::function<void(int)> callback)
{
	m_CloseClientSocketCallback = callback;
}

int DataHandler::GetOnLineUserCount()
{
	return m_OnlineUserMap.size();
}

int DataHandler::GetAvailableThreadCount()
{
	return m_pThreadPool->GetAvailableThreadCount();
}

int DataHandler::GetQueueTaskCount()
{
	return m_pThreadPool->GetTaskCount();
}

int DataHandler::GetAvailableMySqlCount()
{
	return m_pMySqlPool->GetAvailableCount();
}

int DataHandler::UserId2Cfd(int userId)
{
	std::unique_lock<std::mutex> lock(m_OnlineUserMutex);
	if (m_OnlineUserMap.count(userId) == 1)
	{
		return m_OnlineUserMap.at(userId);
	}
	else
	{
		return 0;
	}
}

#define DataTypeMapInsertData(name) \
	m_DataTypeMap.insert( \
		std::map < \
			EDataType, \
			std::function<void(std::shared_ptr<DataPack>&)> \
		>::value_type( \
			EDataType_##name, \
			std::bind(&DataHandler::DataHandler##name, this, std::placeholders::_1) \
		));

void DataHandler::RegeditDataHandler()
{
	DataTypeMapInsertData(C2SLogin);
	DataTypeMapInsertData(C2SDataMsg);
	DataTypeMapInsertData(C2SGetDataMsgList);
}

//登录，c2s 用户id和密码
//返回，
	//1，登录成功的用户名和用户id
	//2，返回所有的用户列表。
	//3，自己的群信息。
void DataHandler::DataHandlerC2SLogin(std::shared_ptr<DataPack>& dataPack)
{
	C2SLogin login;
	S2CLogin loginRet;
	login.UnSerialize(dataPack->pData);
	MySql * mySql = m_pMySqlPool->Pop();
	{
		bool isLogin = DBUser::Login(mySql, login, loginRet);
		std::unique_ptr<char[]> spLogin = loginRet.Serialize();
		SendPackage(dataPack->cfd, spLogin.get(), loginRet.DataSize);

		if(!isLogin){
			//登录失败。
			m_pMySqlPool->Push(mySql);
			return;
		}
	}

	//登录成功：
	ClientOpen(dataPack->cfd, loginRet.UserId);

	{
		//发送所有用户的信息。
		S2CUserList allUser;
		DBUser::AllUserList(mySql, allUser);
		std::unique_ptr<char[]> spAllUser = allUser.Serialize();
		SendPackage(dataPack->cfd, spAllUser.get(), allUser.DataSize);
	}

	{
		//发送自己的群信息。
		S2CGroupList groupList;
		DBGroup::GetUserAllGroup(mySql, loginRet.UserId, groupList);
		std::unique_ptr<char[]> spGroupList = groupList.Serialize();
		SendPackage(dataPack->cfd, spGroupList.get(), groupList.DataSize);
	}
	m_pMySqlPool->Push(mySql);
}

//获取离线消息，用于登录完成后，客户端获取服务器端的离线消息。
void DataHandler::DataHandlerC2SGetDataMsgList(std::shared_ptr<DataPack>& dataPack)
{
	C2SGetDataMsgList getDataMsgList;
	getDataMsgList.UnSerialize(dataPack->pData);
	MySql * mySql = m_pMySqlPool->Pop();

	S2CDataMsgList msgList;
	DBMsg::GetDataMsgList(mySql, getDataMsgList, msgList);

	std::unique_ptr<char[]> msgListPtr = msgList.Serialize();
	SendPackage(dataPack->cfd, msgListPtr.get(), msgList.DataSize);

	m_pMySqlPool->Push(mySql);
}

//
void DataHandler::DataHandlerC2SDataMsg(std::shared_ptr<DataPack>& dataPack)
{
	C2SDataMsg clientMsg;
	clientMsg.UnSerialize(dataPack->pData);
	clientMsg.CreatedTime = Utils::GetTimeStampSeconds();
	MySql * mySql = m_pMySqlPool->Pop();
	
	if (!DBMsg::SaveMsgData(mySql, clientMsg))
	{
		//数据保存到数据库失败。
	}

	std::vector<int> sendUserVector;
	if (clientMsg.IsGroup)
	{
		//群发消息。
		DBGroup::GetUserIdByGroupId(mySql, clientMsg.Receiver, sendUserVector);
	}
	else {
		sendUserVector.push_back(clientMsg.Receiver);
	}

	m_pMySqlPool->Push(mySql);

	S2CDataMsg& serverMsg = *(S2CDataMsg*)&clientMsg;
	serverMsg.DataType = EDataType_S2CDataMsg;

	std::unique_ptr<char[]> serverMsgPtr = serverMsg.Serialize();
	int serverMsgSize = serverMsg.DataSize;

	for (int userId : sendUserVector)
	{
		//过滤自己
		if (userId == clientMsg.Sender)
		{
			continue;
		}
		//单发消息。
		int cfd = UserId2Cfd(userId);
		if (cfd >0)
		{
			SendPackage(cfd, serverMsgPtr.get(), serverMsgSize);
		}
	}
}

void DataHandler::SendPackage(int cfd, char * data, int size)
{
	if (m_SendPackageCallback)
	{
		m_SendPackageCallback(cfd, data, size);
	}
}

void DataHandler::CloseClientSocket(int cfd)
{
	if (m_CloseClientSocketCallback)
	{
		m_CloseClientSocketCallback(cfd);
	}
}
