#ifndef __MY_SQL_H__
#define __MY_SQL_H__

#include <mysql/mysql.h>
#include <vector>
#include <string>


class MySql
{
public:
	MySql();
	MySql(const MySql&) = delete;
	MySql& operator=(MySql&) = delete;
	~MySql();

	bool Init(char * server, char * user, char * password, char* database, int port);

	//执行sql。
	bool ExecSql(char * sql);

	//执行sql返回影响的行数。返回-1，表示错误，返回0，表示没有更新数据，返回大于0表示影响的行数。
	int ExecSqlGetRows(char * sql);

	//执行sql返回插入数据自动生成的id。
	unsigned long long ExeSqlGetId(char * sql);

	bool ExecSqlGetResult(char * sql, std::vector<std::vector<std::string>> & data);

	//关于事务
	//打开事务。
	void AutoCommitOff();
	//提交事务。
	void Commit();
	//回滚
	void Rollback();

	//获取错误信息。
	std::string GetErrInfo();

	//转义字符。
	//参数1，输入的字符串。
	//参数2，转义后的字符串。
	void EscapeString(std::string &from, std::string &ret);

	std::string GetCharSet();

private:
	bool m_IsInit;
	MYSQL * m_pMYSQL;
	bool m_IsAutoMode;

};



#endif // !__MY_SQL_H__
