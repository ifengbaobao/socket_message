#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <vector>


class Logger
{

public:
	enum LoggerLevel
	{
		INFO, DEBUG, WARN, ERROR
	};

private:
	//log 析构
	class LoggerDestory {
	public:
		~LoggerDestory()
		{
			delete GetInstance();
			Logger::s_pInstance = nullptr;
		}
	};

private:
	Logger()
	{
		m_IsDestory = false;
		m_pThread = new std::thread(&Logger::ThreadWork, this);
	}

	Logger(const Logger &) = delete;
	Logger& operator=(const Logger&) = delete;
	
	~Logger()
	{
		m_IsDestory = true;
		m_ConditionVariable.notify_one();
		m_pThread->join();
		delete m_pThread;

		if (m_FileStream.is_open())
		{
			LoggerLevel level = Logger::INFO;
			LogTask(level, "日志文件关闭");
			m_FileStream.close();
		}
	}

	template<typename T, typename ...E>
	void WriteLog(T& t, E& ...e)
	{
		std::cout << t;
		m_FileStream << t;
		WriteLog(e...);
	}

	void WriteLog()
	{
		std::cout << std::endl;
		m_FileStream << std::endl;
	}

	void WriteHeadLog(LoggerLevel level)
	{
		std::string head = GetCurrentSystemTime();
		head += "  [";
		switch (level)
		{
		case INFO:
			head += "INFO";
			break;
		case DEBUG:
			head += "DEBUG";
			break;
		case WARN:
			head += "WARN";
			break;
		case ERROR:
			head += "ERROR";
			break;
		default:
			break;
		}
		head += "]  ";
		std::cout << head;
		m_FileStream << head;
	}
	
	// 获取系统当前时间
	std::string GetCurrentSystemTime()
	{
		auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		struct tm* ptm = localtime(&tt);
		char date[32] = { 0 };
		sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
			(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
			(int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
		return std::string(date);
	}
	
	template<class ...T>
	void LogTask(LoggerLevel& level, T& ... t)
	{
		std::unique_lock<std::mutex> lock(s_Mutex);
		WriteHeadLog(level);
		WriteLog(t...);
	}

	void ThreadWork()
	{
		std::vector<std::function<void()>> tempLogVector;
		while (true)
		{
			{
				std::unique_lock<std::mutex> lock(s_Mutex);
				m_ConditionVariable.wait(lock, [this]()->bool {return (!this->m_LogVector.empty()) || m_IsDestory; });
				for (auto & item: m_LogVector)
				{
					tempLogVector.push_back(std::move(item));
				}
				m_LogVector.clear();
			}
			if (tempLogVector.empty())
			{
				if (m_IsDestory)
				{
					return;
				}
				continue;
			}
			for (auto & item : tempLogVector)
			{
				item();
			}
			tempLogVector.clear();
		}
	}

public:
	static Logger* GetInstance()
	{
		if (s_pInstance == nullptr)
		{
			std::unique_lock<std::mutex> lock(s_Mutex);
			if (s_pInstance == nullptr)
			{
				s_pInstance = new Logger();
				static LoggerDestory ld;
			}
		}
		return Logger::s_pInstance;
	}

	void Init(std::string filePath = std::string())
	{
		if (m_FileStream.is_open())
		{
			return;
		}
		if (filePath.empty())
		{
			filePath += "out.log";
		}
		m_FileStream.open(filePath, std::ios::app | std::ios::out);
		LoggerLevel level = Logger::INFO;
		LogTask(level, "日志文件打开");
	}

	template<class ...T>
	void Log(LoggerLevel level, T ... t)
	{
		std::unique_lock<std::mutex> lock(s_Mutex);
		m_LogVector.push_back(std::bind(&Logger::LogTask<T...>, this, level, t...));
		m_ConditionVariable.notify_one();
	}

private:
	static Logger * s_pInstance;
	static std::mutex s_Mutex;
	std::ofstream m_FileStream;
	std::thread* m_pThread;
	std::condition_variable m_ConditionVariable;
	std::vector<std::function<void()>> m_LogVector;
	bool m_IsDestory;

};


#define LOGI(arg, ...) Logger::GetInstance()->Log(Logger::INFO, arg, ##__VA_ARGS__)
#define LOGD(arg, ...) Logger::GetInstance()->Log(Logger::DEBUG, arg, ##__VA_ARGS__)
#define LOGW(arg, ...) Logger::GetInstance()->Log(Logger::WARN, arg, ##__VA_ARGS__)
#define LOGE(arg, ...) Logger::GetInstance()->Log(Logger::ERROR, arg, ##__VA_ARGS__)

#endif // !__LOGGER_HPP__
