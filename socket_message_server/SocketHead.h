#ifndef __SOCKET_HEAD_H__
#define __SOCKET_HEAD_H__

#include <cstring>

#pragma pack(push)
#pragma pack(1)


struct SocketHead
{
	//包的大小。
	int PackageCount;

	void Serialize(char * dest)
	{
		std::memcpy(dest, this, SocketHeadSize);
	}
	void UnSerialize(char * data)
	{
		std::memcpy(this, data, SocketHeadSize);
	}
	
	static const int SocketHeadSize ;
};


#pragma pack(pop)

#endif // !__SOCKET_HEAD_H__

