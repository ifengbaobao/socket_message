#ifndef __UTILS_H__
#define __UTILS_H__

class Utils
{
private:
	Utils() {};
	Utils(const Utils &) = delete;
	Utils & operator=(Utils &) = delete;
	~Utils() {};


public:
	//获取时间戳，单位秒。
	static int GetTimeStampSeconds();

};



#endif // !__UTILS_H__
