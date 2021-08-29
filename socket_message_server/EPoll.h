#ifndef __E_POLL_H__
#define __E_POLL_H__

#include <functional>

class EPoll
{
public:
	explicit EPoll(int eventSize = 1024);
	EPoll(const EPoll &) = delete;
	EPoll& operator=(const EPoll &) = delete;
	~EPoll();

	//参数，单位毫秒。
	void Wait(int timeout = -1);
	
	//添加
	void Add(int fs, void * data, bool isWrite = false, bool isRead = true);
	//删除
	void Remove(int fs);
	//修改
	void Change(int fs, void * data, bool isWrite = false, bool isRead = true);

	//有消息的回调。第一个参数，数据，第二个参数是否有读，第三个参数是否能写
	void SetEventCallback(std::function<void(void*, bool, bool)> callback);


private:
	int m_Epfd;//epoll 文件描述符。
	int m_EventSize;
	struct epoll_event* m_pEvents;

	//event的回调事件。第一个参数，文件描述符，第二个参数，可读，第三个参数可写
	std::function<void(void*, bool,bool)> m_EventCallback;
};


#endif // !__E_POLL_H__
