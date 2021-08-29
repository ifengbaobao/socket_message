#include "MySql.h"

#include <iostream>



MySql::MySql() :m_IsInit(false), m_pMYSQL(NULL), m_IsAutoMode(true)
{
}


MySql::~MySql()
{
	if (m_IsInit)
	{
		mysql_close(m_pMYSQL);
		delete m_pMYSQL;
	}
}


bool MySql::Init(char * server, char * user, char * password, char* database, int port)
{
	if (m_IsInit)
	{
		return true;
	}
	do {
		m_pMYSQL = new MYSQL();
		if (NULL == mysql_init(m_pMYSQL))
		{
			break;
		}

		// 设置字符集
		if(mysql_set_character_set(m_pMYSQL,"utf8")!= 0)
		{
			break;
		}

		if (NULL == mysql_real_connect(m_pMYSQL, server, user, password, database, port, 0, 0))
		{
			break;
		}

		m_IsInit = true;
		return true;
	} while (0);

	delete m_pMYSQL;
	m_pMYSQL = NULL;
	return false;
}

bool MySql::ExecSql(char * sql)
{
	//std::cout << sql << std::endl;
	return 0 == mysql_query(m_pMYSQL, sql);
}

//执行sql返回影响的行数。
int MySql::ExecSqlGetRows(char * sql)
{
	if (!ExecSql(sql))
	{
		return -1;
	}
	return mysql_affected_rows(m_pMYSQL);
}

unsigned long long MySql::ExeSqlGetId(char * sql)
{
	if (!ExecSql(sql))
	{
		return 0;
	}
	return mysql_insert_id(m_pMYSQL);
}

bool MySql::ExecSqlGetResult(char * sql, std::vector<std::vector<std::string>>& data)
{
	if (!ExecSql(sql))
	{
		return false;
	}

	//获取结果。
	MYSQL_RES * result = mysql_store_result(m_pMYSQL);
	if (result == NULL)
	{
		return false;
	}
	data.clear();

	//列数
	int field = mysql_num_fields(result);
	for (MYSQL_ROW line = mysql_fetch_row(result); line != NULL; line = mysql_fetch_row(result))
	{
		std::vector<std::string> lineData;
		for (int i = 0; i < field; ++i)
		{
			if (line[i])
			{
				lineData.push_back(line[i]);
			}
			else
			{
				lineData.push_back("");
			}
		}
		data.push_back(lineData);
	}
	//释放行结果。
	mysql_free_result(result);
	return true;
}

void MySql::AutoCommitOff()
{
	if (m_IsAutoMode)
	{
		mysql_autocommit(m_pMYSQL, 0);
		m_IsAutoMode = false;
	}
}

void MySql::Commit()
{
	if (!m_IsAutoMode)
	{
		mysql_commit(m_pMYSQL);
		mysql_autocommit(m_pMYSQL, 1);
		m_IsAutoMode = true;
	}
}

void MySql::Rollback()
{
	if (!m_IsAutoMode)
	{
		mysql_rollback(m_pMYSQL);
		mysql_autocommit(m_pMYSQL, 1);
		m_IsAutoMode = true;
	}
}

std::string MySql::GetErrInfo()
{
	unsigned int errorNum = mysql_errno(m_pMYSQL);
	const char * errorInfo = mysql_error(m_pMYSQL);
	std::string info;
	info.append(std::to_string(errorNum));
	info.append("  ");
	info.append(errorInfo);
	return info;
}

void MySql::EscapeString(std::string &from, std::string &ret)
{
	int retSize = from.size() * 2 + 1;
	char * retPtr = new char[retSize];
	mysql_real_escape_string(m_pMYSQL, retPtr, from.c_str(), from.size());
	ret.clear();
	ret.append(retPtr);
	delete[] retPtr;
}

std::string MySql::GetCharSet()
{
	if (m_IsInit) {
		return std::string(mysql_character_set_name(m_pMYSQL));
	}
	else
	{
		return std::string("");
	}
}
