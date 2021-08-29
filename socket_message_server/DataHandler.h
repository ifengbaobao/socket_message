#ifndef __DATA_HANDLER_H__
#define __DATA_HANDLER_H__

#include <map>

#include "ThreadPool.h"
#include "DataType.h"
#include "MySqlPool.h"
#include "SocketControl.h"

struct DataPack
{
	int cfd;
	char* pData;
	int size;

	DataPack()
	{
		cfd = -1;
		pData = nullptr;
		size = 0;
	}

	~DataPack()
	{
		if (pData)
		{
			delete[] pData;
		}
	}
};

class DataHandler
{
public:
	DataHandler(int threadCount = 8,int mySqlPoolCount = 8);
	~DataHandler();

	bool InitMySql(char * server, char * user, char * password, char* database, int port);

	//添加数据。
	void AddData(int cfd, std::unique_ptr<char[]>& data, int size);
	//客户端socket连接状态改变的调用。
	void ClientSocketState(int cfd,bool state);

	//发送数据的回调。
	void SetSendPackageCallback(std::function<void(int, char*, int)> callback);
	//用户在线状态变化的回调，上线下线的回调。
	void SetUserOnlineStateCallback(std::function<void(int, bool)> callback);
	//客户端socket关闭的回调
	void SetCloseClientSocketCallback(std::function<void(int)> callback);

	//获取在线人数。
	int GetOnLineUserCount();
	//获取空线程的个数。可用线程的个数。
	int GetAvailableThreadCount();
	//获取任务队列中等待执行的任务数。
	int GetQueueTaskCount();
	//获取可用的MySql的数量。
	int GetAvailableMySqlCount();


private:
	void ThreadWork(std::shared_ptr<DataPack>& dataPack);

	void ClientOpen(int cfd,int userId);

	//用户id转换为用户所在的客户端cfd。
	int UserId2Cfd(int userId);

	void RegeditDataHandler();

	//登录。
	void DataHandlerC2SLogin(std::shared_ptr<DataPack>& dataPack);
	//获取一组消息，用于登录成功后，获取离线消息的。
	void DataHandlerC2SGetDataMsgList(std::shared_ptr<DataPack>& dataPack);
	//客户端发送了一条消息。
	void DataHandlerC2SDataMsg(std::shared_ptr<DataPack>& dataPack);

	//发送数据。
	void SendPackage(int cfd,char * data,int size);

	//关闭客户端。用于在某些情况下，需要服务器端主动关闭客户端的时候。
	void CloseClientSocket(int cfd);

private:
	ThreadPool* m_pThreadPool;
	std::map < EDataType, std::function<void(std::shared_ptr<DataPack>&)> > m_DataTypeMap;
	MySqlPool* m_pMySqlPool;
	//在线用户的map，第1个int，UserId。第2个int，客户端cfd。
	std::map<int, int> m_OnlineUserMap;
	//在线用户的锁，
	std::mutex m_OnlineUserMutex;
	//发送数据包的回调。
	std::function<void(int,char*,int)> m_SendPackageCallback;
	//用户上线下线的回调。
	std::function<void(int, bool)> m_UserOnlineStateCallback;
	//客户端socket关闭的回调
	std::function<void(int)> m_CloseClientSocketCallback;
};



#endif // !__DATA_HANDLER_H__
