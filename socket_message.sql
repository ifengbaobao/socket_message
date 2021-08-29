/*
*********************************************************************
用户表：
	UserId			用户id
	UserName		用户名
群组表：
	GroupId			群id
	GroupName		群名字
	AdminId			管理员id
群组和用户的关联表：
	GroupId			群id
	UserId			用户id
消息表：
	MsgId			消息id
	MsgType			消息类型，文本，命令，图片。。。
	Msg				消息。		TEXT
	Sender			发送者
	Receiver		接收者
	IsGroup			接收者是否群组。
	CreatedTime		创建时间
*********************************************************************
*/
DROP DATABASE IF EXISTS `socket_message`;

CREATE DATABASE `socket_message` CHARACTER SET utf8 COLLATE utf8_bin; 

USE `socket_message`;

DROP TABLE IF EXISTS `User`;

CREATE TABLE `User`( 
	`UserId` INT NOT NULL AUTO_INCREMENT COMMENT '用户id', 
	`UserName` VARCHAR(32) COMMENT '用户名', 
	`Password` VARCHAR(32) COMMENT '密码', 
	PRIMARY KEY (`UserId`)
) ENGINE='Default' COMMENT='用户表' ROW_FORMAT=DEFAULT CHARSET=utf8 COLLATE=utf8_bin AUTO_INCREMENT=100000;

DROP TABLE IF EXISTS `Group`;

CREATE TABLE `Group`( 
	`GroupId` INT NOT NULL AUTO_INCREMENT COMMENT '群id', 
	`GroupName` VARCHAR(32) COMMENT '群名', 
	`AdminId` INT COMMENT '管理员id', 
	PRIMARY KEY (`GroupId`)
) ENGINE='Default' COMMENT='群表' ROW_FORMAT=DEFAULT CHARSET=utf8 COLLATE=utf8_bin AUTO_INCREMENT=1000;

DROP TABLE IF EXISTS `GroupUser`;

CREATE TABLE `GroupUser`( 
	`GroupId` INT NOT NULL COMMENT '群id', 
	`UserId` INT NOT NULL COMMENT '用户id'
) ENGINE='Default' COMMENT='群中的用户表' ROW_FORMAT=DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

DROP TABLE IF EXISTS `Msg`;

CREATE TABLE `Msg`( 
	`MsgId` INT NOT NULL AUTO_INCREMENT COMMENT '消息id', 
	`MsgType` INT COMMENT '消息类型', 
	`Msg` TEXT COMMENT '消息内容', 
	`Sender` INT COMMENT '发送者',
	`Receiver` INT COMMENT '接收者',
	`IsGroup` INT COMMENT '接收者是否群组',
	`CreatedTime` INT COMMENT '创建时间', 
	`Extend` TEXT COMMENT '扩展内容', 
	PRIMARY KEY (`MsgId`)
) ENGINE='Default' COMMENT='消息表' ROW_FORMAT=DEFAULT CHARSET=utf8 COLLATE=utf8_bin AUTO_INCREMENT=1000000;


/* 测试数据: */

INSERT INTO `User` ( `UserName`, `Password`) VALUES ( 'Test1', '000000');
INSERT INTO `User` ( `UserName`, `Password`) VALUES ( 'Test2', '000000');
INSERT INTO `User` ( `UserName`, `Password`) VALUES ( 'Test3', '000000');
INSERT INTO `User` ( `UserName`, `Password`) VALUES ( 'Test4', '000000');
INSERT INTO `User` ( `UserName`, `Password`) VALUES ( 'Test5', '000000');
INSERT INTO `User` ( `UserName`, `Password`) VALUES ( 'Test6', '000000');
INSERT INTO `User` ( `UserName`, `Password`) VALUES ( 'Test7', '000000');
INSERT INTO `User` ( `UserName`, `Password`) VALUES ( 'Test8', '000000');
INSERT INTO `User` ( `UserName`, `Password`) VALUES ( 'Test9', '000000');


INSERT INTO `Group` ( `GroupName`, `AdminId`) VALUES ( 'Group1', '100000');
INSERT INTO `Group` ( `GroupName`, `AdminId`) VALUES ( 'Group2', '100000');
INSERT INTO `Group` ( `GroupName`, `AdminId`) VALUES ( 'Group3', '100001');


INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1000', '100000');
INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1000', '100001');
INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1000', '100002');
INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1000', '100003');
INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1000', '100004');

INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1001', '100000');
INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1001', '100002');
INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1001', '100003');
INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1001', '100004');

INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1002', '100001');
INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1002', '100002');
INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1002', '100003');
INSERT INTO `GroupUser` (`GroupId`, `UserId`) VALUES ('1002', '100004');
