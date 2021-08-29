#include "DBMsg.h"


void DBMsg::GetDataMsgList(MySql * mySql, C2SGetDataMsgList & getDataMsgList, S2CDataMsgList & ret)
{
	int limit = 100;
	char where[32] = {};
	//说明之前有数据。
	if (getDataMsgList.LastMsgId > 0)
	{
		std::sprintf(where, "AND `MsgId` > '%d'", getDataMsgList.LastMsgId);
	}
	else
	{
		std::sprintf(where, "AND 1=1 LIMIT 0, %d", limit);
	}

	char sql[256] = {};

	std::sprintf(sql, "SELECT `MsgId`, `MsgType`, `Msg`, `Sender`, `Receiver`, `IsGroup`, `CreatedTime`, `Extend` FROM `Msg` WHERE ( `Receiver` = '%d' OR `Receiver` IN ( SELECT `GroupId` FROM `GroupUser` WHERE `UserId` = '%d' )) %s"
		,getDataMsgList.UserId
		,getDataMsgList.UserId
		,where
	);

	std::vector<std::vector<std::string>> sqlData;
	mySql->ExecSqlGetResult(sql, sqlData);

	for (std::vector<std::string> & lineSqlData : sqlData)
	{
		S2CDataMsg msg;
		msg.MsgId = std::stoi(lineSqlData.at(0));
		msg.MsgType = (EMsgType)std::stoi(lineSqlData.at(1));
		msg.Msg.append(lineSqlData.at(2));	// '消息内容',
		msg.Sender = std::stoi(lineSqlData.at(3)); //'发送者',
		msg.Receiver = std::stoi(lineSqlData.at(4)); // '接收者',
		msg.IsGroup = std::stoi(lineSqlData.at(5)); // '接收者是否群组',  5
		msg.CreatedTime = std::stoi(lineSqlData.at(6)); //'创建时间',     6
		msg.Extend.append(lineSqlData.at(7));	//'扩展内容',          7
		ret.MsgVector.push_back(msg);
	}
}

bool DBMsg::SaveMsgData(MySql * mySql, C2SDataMsg & clientMsg)
{

	std::string msg;
	mySql->EscapeString(clientMsg.Msg,msg);

	std::string ext;
	mySql->EscapeString(clientMsg.Extend,ext);

	char* sql = new char[256+msg.size()+ext.size()];

	std::sprintf(sql, "INSERT INTO `Msg` ( `MsgType`, `Sender`, `Receiver`, `IsGroup`, `CreatedTime`, `Msg`, `Extend`) VALUES ( '%d', '%d', '%d', '%d', '%d', '%s', '%s');"
		,(int)clientMsg.MsgType
		,clientMsg.Sender
		,clientMsg.Receiver
		,(int)clientMsg.IsGroup
		,clientMsg.CreatedTime
		,msg.c_str()
		,ext.c_str()
	);
	clientMsg.MsgId = mySql->ExeSqlGetId(sql);
	delete[] sql;
	return clientMsg.MsgId != 0;
}

