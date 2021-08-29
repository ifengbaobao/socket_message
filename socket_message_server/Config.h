﻿#ifndef __CONFIG_H__
#define __CONFIG_H__

//socket 线程的个数。
#define CONFIG_SOCKET_THREAD_COUNT 8

//数据处理的线程的个数。
#define CONFIG_DATA_HANDLER_THREAD_COUNT 8

//mysql连接池的连接个数。
#define CONFIG_MYSQL_CONNECT_COUNT (CONFIG_DATA_HANDLER_THREAD_COUNT + 1)

//mysql 的连接信息。
#define CONFIG_MYSQL_SERVER "localhost"
#define CONFIG_MYSQL_USER "root"
#define CONFIG_MYSQL_PASSWORD "123456"
#define CONFIG_MYSQL_DATABASE "socket_message"
#define CONFIG_MYSQL_PORT 3306

//epoll 的超时时间 单位毫秒。
#define CONFIG_EPOLL_TIMEOUT 1000

//服务器端 socket 监听连接的端口。
#define CONFIG_SERVICE_SOCKET_PORT 1234


//客户端缓存的最大大小。
#define CONFIG_CLIENT_READBUF_MAX_SIZE 100*1024
#define CONFIG_CLIENT_WRITEBUF_MAX_SIZE 100*1024

//关于内存池。
#define CONFIG_MEMORY_POOL_8_COUNT 2048
#define CONFIG_MEMORY_POOL_16_COUNT 2048
#define CONFIG_MEMORY_POOL_32_COUNT 102400
#define CONFIG_MEMORY_POOL_64_COUNT 2048
#define CONFIG_MEMORY_POOL_128_COUNT 1024
#define CONFIG_MEMORY_POOL_256_COUNT 2048
#define CONFIG_MEMORY_POOL_512_COUNT 1024
#define CONFIG_MEMORY_POOL_1024_COUNT 1024
#define CONFIG_MEMORY_POOL_2048_COUNT 1024

#endif // !__CONFIG_H__

