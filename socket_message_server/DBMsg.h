#ifndef __DB_MSG_H__
#define __DB_MSG_H__

#include "DataType.h"
#include "MySql.h"

class DBMsg
{
private:
	DBMsg() = delete;
	DBMsg(const DBMsg&) = delete;
	DBMsg& operator=(DBMsg&) = delete;
	~DBMsg() = delete;

public:
	//获取用户的离线消息用。
	static void GetDataMsgList(MySql * mySql, C2SGetDataMsgList & getDataMsgList, S2CDataMsgList& ret);
	
	//消息保存到数据库。
	static bool SaveMsgData(MySql * mySql, C2SDataMsg & clientMsg);


};

#endif // !__DB_MSG_H__
