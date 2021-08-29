#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

#include <vector>
#include <memory>
#include <cstring>

#include "SocketMessageSdkDataType.h"

#pragma pack(push)
#pragma pack(1)

//默认的数据序列化。
#define DefaultDataSerializeAndUnSerialize \
	std::unique_ptr<char[]> Serialize() \
	{ \
		char * p = new char[DataSize]; \
		std::memcpy(p, this, DataSize); \
		return std::unique_ptr<char[]>(p); \
	} \
	void UnSerialize(char * data) \
	{ \
			std::memcpy(this, data, DataSize); \
	}



enum EDataType
{
	//客户端向服务器端发送的数据。
	EDataType_C2SLogin = 100,
	EDataType_C2SDataMsg,
	EDataType_C2SGetDataMsgList,

	//服务器端向客户端发送的数据。
	EDataType_S2CLogin = 200,
	EDataType_S2CUser,
	EDataType_S2CUserList,
	EDataType_S2CGroup,
	EDataType_S2CGroupList,
	EDataType_S2CDataMsg,
	EDataType_S2CDataMsgList,
};


struct DataHead
{
	int DataSize;
	EDataType DataType;
};

//客户端登录
struct C2SLogin :public DataHead
{
	int UserId;
	char Password[32];

	C2SLogin()
	{
		DataSize = sizeof(C2SLogin);
		DataType = EDataType_C2SLogin;
	}

	DefaultDataSerializeAndUnSerialize
};

//客户端登录服务器返回信息。
struct S2CLogin :public DataHead
{
	int UserId;
	char UserName[32];

	S2CLogin()
	{
		DataSize = sizeof(S2CLogin);
		DataType = EDataType_S2CLogin;
	}

	DefaultDataSerializeAndUnSerialize
};

//服务器向客户端返回单个用户。
struct S2CUser :public DataHead
{
	int UserId;
	char UserName[32];

	S2CUser()
	{
		DataSize = sizeof(S2CUser);
		DataType = EDataType_S2CUser;
	}

	DefaultDataSerializeAndUnSerialize

};

//服务器向客户端返回用户列表。
struct S2CUserList : public DataHead
{
	std::vector<S2CUser> UserVector;

	S2CUserList()
	{
		DataSize = 0;
		DataType = EDataType_S2CUserList;
	}

	std::unique_ptr<char[]> Serialize()
	{
		DataSize = sizeof(DataHead) + sizeof(int) + sizeof(S2CUser) * UserVector.size();
		char * p = new char[DataSize];
		int userVectorCount = UserVector.size();
		std::memcpy(p, this, sizeof(DataHead));
		std::memcpy(p + sizeof(DataHead), &userVectorCount, sizeof(int));
		int offset = sizeof(DataHead) + sizeof(int);
		for (S2CUser & user : UserVector)
		{
			std::memcpy(p + offset, &user, sizeof(S2CUser));
			offset += sizeof(S2CUser);
		}
		return std::unique_ptr<char[]>(p);
	}

	void UnSerialize(char * data)
	{
		std::memcpy(this, data, sizeof(DataHead));
		int userVectorCount = *(data + sizeof(DataHead));
		int offset = sizeof(DataHead) + sizeof(int);
		for (int i = 0; i < userVectorCount; ++i)
		{
			S2CUser user;
			std::memcpy(&user, data + offset, sizeof(S2CUser));
			UserVector.push_back(user);
			offset += sizeof(S2CUser);
		}
	}

};

//服务器向客户端返回群组信息。
struct S2CGroup :public DataHead
{
	int GroupId;
	char GroupName[32];
	int AdminId;
	S2CUserList UserList;

	S2CGroup()
	{
		DataSize = 0;
		DataType = EDataType_S2CGroup;
	}


	std::unique_ptr<char[]> Serialize()
	{
		std::unique_ptr<char[]> pUser = UserList.Serialize();
		int pUserCount = UserList.DataSize;

		int size1 = sizeof(DataHead) + sizeof(int) + sizeof(GroupName) + sizeof(int);
		DataSize = size1 + pUserCount;
		char * p = new char[DataSize];
		std::memcpy(p, this, size1);
		std::memcpy(p + size1, pUser.get(), pUserCount);
		return std::unique_ptr<char[]>(p);
	}

	void UnSerialize(char * data)
	{
		int size1 = sizeof(DataHead) + sizeof(int) + sizeof(GroupName) + sizeof(int);
		std::memcpy(this, data, size1);
		UserList.UnSerialize(data + size1);
	}
};

//服务器向客户端返回的群组列表。
struct S2CGroupList : public DataHead
{
	std::vector<S2CGroup> GroupVector;

	S2CGroupList()
	{
		DataSize = 0;
		DataType = EDataType_S2CGroupList;
	}

	std::unique_ptr<char[]> Serialize()
	{
		int GroupVectorSize = 0;
		std::vector<std::unique_ptr<char[]>> GroupVectorPtr;
		for (S2CGroup & group : GroupVector)
		{
			GroupVectorPtr.push_back(group.Serialize());
			GroupVectorSize += group.DataSize;
		}

		DataSize = sizeof(DataHead) + sizeof(int) + GroupVectorSize;
		char * p = new char[DataSize];
		std::memcpy(p, this, sizeof(DataHead));
		int GroupVectorCount = GroupVector.size();
		std::memcpy(p + sizeof(DataHead), &GroupVectorCount, sizeof(int));
		int offset = sizeof(DataHead) + sizeof(int);
		for (int i = 0; i < GroupVectorCount; ++i)
		{
			std::memcpy(p + offset, GroupVectorPtr.at(i).get(), GroupVector.at(i).DataSize);
			offset += GroupVector.at(i).DataSize;
		}
		return std::unique_ptr<char[]>(p);
	}

	void UnSerialize(char * data)
	{
		std::memcpy(this, data, sizeof(DataHead));
		int GroupVectorCount = *(data + sizeof(DataHead));
		int offset = sizeof(DataHead) + sizeof(int);
		for (int i = 0; i < GroupVectorCount; ++i)
		{
			S2CGroup Group;
			Group.UnSerialize(data + offset);
			GroupVector.push_back(Group);
			offset += Group.DataSize;
		}
	}

};


struct C2SGetDataMsgList :public DataHead
{
	int LastMsgId;
	int UserId;
	C2SGetDataMsgList()
	{
		DataSize = sizeof(C2SGetDataMsgList);
		DataType = EDataType_C2SGetDataMsgList;
	}

	DefaultDataSerializeAndUnSerialize
};

struct DataMsg :public DataHead
{
	int MsgId;//消息id
	EMsgType MsgType;// '消息类型',
	int Sender; //'发送者',
	int Receiver; // '接收者',
	bool IsGroup; // '接收者是否群组',
	int CreatedTime; //'创建时间',
	std::string Msg;	// '消息内容',
	std::string Extend;	//'扩展内容',

	std::unique_ptr<char[]> Serialize()
	{
		int size1 = sizeof(DataHead) + sizeof(int) + sizeof(EMsgType) + sizeof(int) + sizeof(int) + sizeof(bool) + sizeof(int);
		DataSize = size1 + sizeof(int) + Msg.size() + sizeof(int) + Extend.size()+2;

		char * p = new char[DataSize];
		std::memcpy(p, this, size1);

		int offset = size1;

		int MsgSize = Msg.size() + 1;
		memcpy(p + offset, &MsgSize, sizeof(int));

		offset += sizeof(int);
		memcpy(p + offset, Msg.c_str(), MsgSize);
		offset += MsgSize;

		int ExtendSize = Extend.size() + 1;
		memcpy(p + offset, &ExtendSize, sizeof(int));
		offset += sizeof(int);
		memcpy(p + offset, Extend.c_str(), ExtendSize);

		return std::unique_ptr<char[]>(p);
	}
	
	char * Serialize2()
	{
		int size1 = sizeof(DataHead) + sizeof(int) + sizeof(EMsgType) + sizeof(int) + sizeof(int) + sizeof(bool) + sizeof(int);
		DataSize = size1 + sizeof(int) + Msg.size() + sizeof(int) + Extend.size()+2;

		char * p = new char[DataSize];
		std::memcpy(p, this, size1);

		int offset = size1;

		int MsgSize = Msg.size() + 1;
		memcpy(p + offset, &MsgSize, sizeof(int));

		offset += sizeof(int);
		memcpy(p + offset, Msg.c_str(), MsgSize);
		offset += MsgSize;

		int ExtendSize = Extend.size() + 1;
		memcpy(p + offset, &ExtendSize, sizeof(int));
		offset += sizeof(int);
		memcpy(p + offset, Extend.c_str(), ExtendSize);

		return p;
	}

	void UnSerialize(char * data)
	{
		int size1 = sizeof(DataHead) + sizeof(int) + sizeof(EMsgType) + sizeof(int) + sizeof(int) + sizeof(bool) + sizeof(int);
		std::memcpy(this, data, size1);

		int offset = size1;
		int MsgSize = *(data + offset);
		offset += sizeof(int);
		Msg.append(data + offset);
		offset += MsgSize;
		++offset;

		int ExtendSize = *(data + offset);
		offset += sizeof(int);
		Extend.append(data + offset);
	}
};

struct C2SDataMsg : public DataMsg
{
	C2SDataMsg()
	{
		DataSize = 0;
		DataType = EDataType_C2SDataMsg;

	}
};

struct S2CDataMsg : public DataMsg
{
	S2CDataMsg()
	{
		DataSize = 0;
		DataType = EDataType_S2CDataMsg;
	}
};

struct DataMsgList : public DataHead
{
	std::vector<DataMsg> MsgVector;

	std::unique_ptr<char[]> Serialize()
	{
		int MsgVectorSize = 0;
		std::vector<std::unique_ptr<char[]>> MsgVectorPtr;
		for (DataMsg & Msg : MsgVector)
		{
			MsgVectorPtr.push_back(Msg.Serialize());
			MsgVectorSize += Msg.DataSize;
		}

		DataSize = sizeof(DataHead) + sizeof(int) + MsgVectorSize;
		char * p = new char[DataSize];
		std::memcpy(p, this, sizeof(DataHead));
		int MsgVectorCount = MsgVector.size();
		std::memcpy(p + sizeof(DataHead), &MsgVectorCount, sizeof(int));
		int offset = sizeof(DataHead) + sizeof(int);
		for (int i = 0; i < MsgVectorCount; ++i)
		{
			std::memcpy(p + offset, MsgVectorPtr.at(i).get(), MsgVector.at(i).DataSize);
			offset += MsgVector.at(i).DataSize;
		}
		return std::unique_ptr<char[]>(p);
	}

	void UnSerialize(char * data)
	{
		std::memcpy(this, data, sizeof(DataHead));
		int MsgVectorCount = *(data + sizeof(DataHead));
		int offset = sizeof(DataHead) + sizeof(int);
		for (int i = 0; i < MsgVectorCount; ++i)
		{
			DataMsg Msg;
			Msg.UnSerialize(data + offset);
			MsgVector.push_back(Msg);
			offset += Msg.DataSize;
		}
	}
};

struct S2CDataMsgList : public DataMsgList
{
	S2CDataMsgList()
	{
		DataSize = 0;
		DataType = EDataType_S2CDataMsgList;
	}
};



#pragma pack(pop)

#endif // !__DATA_TYPE_H__

