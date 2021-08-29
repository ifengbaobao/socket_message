#ifndef __CHARSET_UTILS_H__
#define __CHARSET_UTILS_H__

#include <string>

class CharsetUtils
{
private:
	CharsetUtils();
	CharsetUtils(const CharsetUtils &) = delete;
	CharsetUtils& operator=(CharsetUtils &) = delete;
	~CharsetUtils();

	//关于转码的。
private:
#ifdef _WIN32
#else
	static int CodeConvert(const char *from_charset, const char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen);
#endif

public:
	static std::string GbkToUtf8(std::string & gStr);
	static std::string Utf8ToGbk(std::string & uStr);

};


#endif // !__CHARSET_UTILS_H__
