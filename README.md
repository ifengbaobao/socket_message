# socket_message

## 项目简介

socket的高并发的一个传统的实现。
服务端的内部实现用的epoll。
客户的端的实现用的select。
发送的消息会保存到mysql数据库。
本项目内部传输的数据，是简单的定义了几个im的消息。

## 项目包括

**服务端。**
仅可以在linux下运行。
需要线安装 mysqlclient 库。
内部实现用的epoll。
并发可以支持到上万个连接。
服务端运行前，需要线导入 socket_message.sql sql脚本到mysql数据库中。

**客户端。**
客户端包括：客户端sdk，和windows下和linux都可以运行的sdk调用demo。
客户端sdk是跨平台的，用的select对socket的处理。
目前可以在windows linux android，这三个平台运行。

**android客户端**
通过jni调用客户端的sdk，实现长连接。

## 作者

微信：383537473  如需添加，说明暗号：socket_message
