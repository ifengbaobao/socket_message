#ifndef __SOCKET_INCLUDE_HEAD_H__
#define __SOCKET_INCLUDE_HEAD_H__

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
//WIN32_LEAN_AND_MEAN 宏定义的目的 winsock2.h和windows.h之间有关于_WINSOCKAPI_的重复定义
#define WIN32_LEAN_AND_MEAN	
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

//windows中socket的库。
#pragma comment(lib,"ws2_32.lib")

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

//typedef unsigned int SOCKET
//#define INVALID_SOCKET  (SOCKET)(~0)

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(-1)

#endif


#endif // !__SOCKET_INCLUDE_HEAD_H__

