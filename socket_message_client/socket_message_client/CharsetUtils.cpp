#include "CharsetUtils.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <iconv.h>
#include <cstring>
#include <memory>
#endif


CharsetUtils::CharsetUtils()
{
}


CharsetUtils::~CharsetUtils()
{
}



#ifdef _WIN32

std::string CharsetUtils::GbkToUtf8(std::string & gStr)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gStr.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gStr.c_str(), -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	std::string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}

std::string CharsetUtils::Utf8ToGbk(std::string & uStr)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, uStr.c_str(), -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, uStr.c_str(), -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}
#else



int CharsetUtils::CodeConvert(const char * from_charset, const char * to_charset, char * inbuf, size_t inlen, char * outbuf, size_t outlen)
{
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
		return -1;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
	{

		iconv_close(cd);
		return -1;
	}
	else
	{
		iconv_close(cd);
		return 0;
	}
}


std::string CharsetUtils::GbkToUtf8(std::string & gStr)
{
	size_t inLength = gStr.size();
	size_t outLength = gStr.size() * 2 + 1;
	char* outBuf = new char[outLength] {0};
	std::shared_ptr<char[]> outPtr(outBuf);

	CodeConvert("gb2312", "utf-8", (char *)gStr.c_str(), inLength, outBuf, outLength);
	return std::string(outBuf);
}

std::string CharsetUtils::Utf8ToGbk(std::string & uStr)
{
	size_t inLength = uStr.size();
	size_t outLength = inLength + 1;
	char *outBuf = new char[outLength] {0};
	std::shared_ptr<char[]> outPtr(outBuf);

	CodeConvert("utf-8", "gb2312", (char*)uStr.c_str(), inLength, outBuf, outLength);
	return std::string(outBuf);
}


#endif