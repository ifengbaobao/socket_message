#ifndef __SOCKET_MESSAGE_SDK_DATA_TYPE_H__
#define __SOCKET_MESSAGE_SDK_DATA_TYPE_H__

#include <vector>
#include <string>


//消息的类型
enum EMsgType
{
	MsgTypeText = 1//文本类型。
	//后续可以扩展图片，文件，命令，等等。
};


//用户信息。
struct UserInfo
{
	int UserId;
	std::string UserName;
};

struct GroupInfo
{
	int GroupId;
	std::string GroupName;
	int AdminId;
	std::vector<UserInfo> UserList;
};

struct ChatMsg
{
	int MsgId;//消息id
	EMsgType MsgType;// '消息类型',
	int Sender; //'发送者',
	int Receiver; // '接收者',
	bool IsGroup; // '接收者是否群组',
	int CreatedTime; //'创建时间',
	std::string Msg;	// '消息内容',
	std::string Extend;	//'扩展内容',
};



#endif // !__SOCKET_MESSAGE_SDK_DATA_TYPE_H__

