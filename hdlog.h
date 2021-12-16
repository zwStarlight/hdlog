#ifndef HDLOG_H
#define HDLOG_H

#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <chrono>
#include <memory>
#include <stdarg.h>

#define RESET "\033[0m"
#define BLACK "\033[30m" /* Black */
#define RED "\033[31m" /* Red */
#define GREEN "\033[32m" /* Green */
#define YELLOW "\033[33m" /* Yellow */
#define BLUE "\033[34m" /* Blue */
#define MAGENTA "\033[35m" /* Magenta */
#define CYAN "\033[36m" /* Cyan */
#define WHITE "\033[37m" /* White */
#define BOLDBLACK "\033[1m\033[30m" /* Bold Black */
#define BOLDRED "\033[1m\033[31m" /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m" /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m" /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m" /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m" /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m" /* Bold White */
#endif

#include <time.h>
#include <iostream>
#include <mutex>
#include <vector>
#include <string>
#include <sstream>
#include <exception>

namespace hdlog
{
	enum class LogLevel
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error
	};

	static std::mutex io_mutex;
	static LogLevel level = LogLevel::Info;
	static std::string pattern = "{}";

	inline std::string GetSystemTime()
	{
#ifdef _WIN32
		time_t tNowTime;
		time(&tNowTime);
		//struct tm t;
		tm* tLocalTime = localtime(&tNowTime);
		char szTime[30] = { '\0' };
		strftime(szTime, 30, "[%Y-%m-%d %H:%M:%S", tLocalTime);
		std::string strTime = szTime;

		SYSTEMTIME t;

		GetLocalTime(&t);
		int ms = t.wMilliseconds;

		if (ms < 10)
		{
			strTime += ".00" + std::to_string(t.wMilliseconds);
		}
		else if (ms < 100)
		{
			strTime += ".0" + std::to_string(t.wMilliseconds);
		}
		else
		{
			strTime += "." + std::to_string(t.wMilliseconds);
		}

		strTime += "]";

		return strTime;
#else
		std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
		auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
		std::time_t timestamp = tmp.count();

		uint64_t milli = timestamp;
		milli += (uint64_t)8 * 60 * 60 * 1000;
		auto mTime = std::chrono::milliseconds(milli);
		tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(mTime);
		auto tt = std::chrono::system_clock::to_time_t(tp);
		std::tm* now = std::gmtime(&tt);

		char rst[27] = { 0 };
		sprintf(rst, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, std::atoi(std::to_string(timestamp % 1000000).substr(3, 3).c_str()));

		return rst;
#endif
	}

	inline std::vector<std::string> split(std::string str, std::string pattern)
	{
		std::string::size_type pos;
		std::vector<std::string> result;
		str += pattern;//扩展字符串以方便操作
		int size = str.size();

		for (int i = 0; i < size; i++)
		{
			pos = str.find(pattern, i);
			if (pos < size)
			{
				std::string s = str.substr(i, pos - i);
				result.push_back(s);
				i = pos + pattern.size() - 1;
			}
		}
		return result;
	}

	inline void SetLevel(LogLevel Level)
	{
		std::lock_guard<std::mutex> guard(io_mutex);
		level = Level;
	}

	inline void SetPattern(std::string pat)
	{
		std::lock_guard<std::mutex> guard(io_mutex);
		pattern = pat;
	}

	inline std::ostream& Red(std::ostream& s)
	{
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
#else
		s << BOLDRED;
#endif
		return s;
	}

	inline std::ostream& Yellow(std::ostream& s)
	{
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
#else
		s << BOLDYELLOW;
#endif
		return s;
	}

	inline std::ostream& Blue(std::ostream& s)
	{
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
#else
		s << BOLDBLUE;
#endif
		return s;
	}

	inline std::ostream& Green(std::ostream& s)
	{
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
#else
		s << BOLDGREEN;
#endif
		return s;
	}

	inline std::ostream& White(std::ostream& s)
	{
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
#else
		s << BOLDWHITE;
#endif
		return s;
	}

	inline std::ostream& Reset(std::ostream& s)
	{
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
#else
		s << RESET;
#endif
		return s;
	}

	template<typename T>
	inline void readParas(std::vector<std::string>& content, T&& args)
	{
		std::stringstream ss;
		ss << args;
		std::string s = ss.str();
		content.push_back(s);
	}

	inline void Log(std::vector<std::string>& content)
	{
	}

	template<typename T, typename... Args>
	inline void Log(std::vector<std::string>& content, T&& first, Args&&... args)
	{	
		readParas(content, std::forward<T>(first));
		Log(content, std::forward<Args>(args)...);
	}

	template<typename... Args>
	inline void trace(std::string content, Args... args)
	{
		try
		{
			std::lock_guard<std::mutex> guard(io_mutex);
			if (level > LogLevel::Trace) return;

			std::vector<std::string> cs = split(content, pattern);

			int paramcount = 0;
			paramcount = sizeof...(args);

			int amount = cs.size();

			if (paramcount == 0 || amount == 1)
			{
				std::cout << GetSystemTime().c_str() << " [" << White << "Trace" << Reset << "] " << content << std::endl;
			}
			else
			{
				if (amount - 1 < paramcount)
				{
					std::vector<std::string> paramlist;

					Log(paramlist, args...);

					std::cout << GetSystemTime().c_str() << " [" << White << "Trace" << Reset << "] ";

					for (int i = 0; i < amount - 1; ++i)
					{
						std::cout << cs[i] << paramlist[i];
					}

					std::cout << cs[amount - 1] << std::endl;
				}
				else
				{
					std::vector<std::string> paramlist;

					Log(paramlist, args...);

					std::cout << GetSystemTime().c_str() << " [" << White << "Trace" << Reset << "] ";

					for (int i = 0; i < paramcount; ++i)
					{
						std::cout << cs[i] << paramlist[i];
					}

					for (int i = paramcount; i < amount - 1; ++i)
					{
						std::cout << cs[i] << pattern;
					}

					std::cout << cs[amount - 1] << std::endl;
				}
			}
		}
		catch (std::exception& ex)
		{
			std::cout << GetSystemTime().c_str() << " [" << Red << "Error" << Reset << "] " << ex.what() << std::endl;
		}
	}

	template<typename... Args>
	inline void debug(std::string content, Args... args)
	{
		try
		{
			std::lock_guard<std::mutex> guard(io_mutex);
			if (level > LogLevel::Debug) return;

			std::vector<std::string> cs = split(content, pattern);

			int paramcount = 0;
			paramcount = sizeof...(args);

			int amount = cs.size();

			if (paramcount == 0 || amount == 1)
			{
				std::cout << GetSystemTime().c_str() << " [" << Blue << "Debug" << Reset << "] " << content << std::endl;
			}
			else
			{
				if (amount - 1 < paramcount)
				{
					std::vector<std::string> paramlist;

					Log(paramlist, args...);

					std::cout << GetSystemTime().c_str() << " [" << Blue << "Debug" << Reset << "] ";

					for (int i = 0; i < amount - 1; ++i)
					{
						std::cout << cs[i] << paramlist[i];
					}

					std::cout << cs[amount - 1] << std::endl;
				}
				else
				{
					std::vector<std::string> paramlist;

					Log(paramlist, args...);

					std::cout << GetSystemTime().c_str() << " [" << Blue << "Debug" << Reset << "] ";

					for (int i = 0; i < paramcount; ++i)
					{
						std::cout << cs[i] << paramlist[i];
					}

					for (int i = paramcount; i < amount - 1; ++i)
					{
						std::cout << cs[i] << pattern;
					}

					std::cout << cs[amount - 1] << std::endl;
				}
			}
		}
		catch (std::exception& ex)
		{
			std::cout << GetSystemTime().c_str() << " [" << Red << "Error" << Reset << "] " << ex.what() << std::endl;
		}
	}

	template<typename... Args>
	inline void info(std::string content, Args... args)
	{
		try
		{
			std::lock_guard<std::mutex> guard(io_mutex);
			if (level > LogLevel::Info) return;

			std::vector<std::string> cs = split(content, pattern);

			int paramcount = 0;
			paramcount = sizeof...(args);

			int amount = cs.size();

			if (paramcount == 0 || amount == 1)
			{
				std::cout << GetSystemTime().c_str() << " [" << Green << "Info" << Reset << "] " << content << std::endl;
			}
			else
			{
				if (amount - 1 < paramcount)
				{
					std::vector<std::string> paramlist;

					Log(paramlist, args...);

					std::cout << GetSystemTime().c_str() << " [" << Green << "Info" << Reset << "] ";

					for (int i = 0; i < amount - 1; ++i)
					{
						std::cout << cs[i] << paramlist[i];
					}

					std::cout << cs[amount - 1] << std::endl;
				}
				else
				{
					std::vector<std::string> paramlist;

					Log(paramlist, args...);

					std::cout << GetSystemTime().c_str() << " [" << Green << "Info" << Reset << "] ";

					for (int i = 0; i < paramcount; ++i)
					{
						std::cout << cs[i] << paramlist[i];
					}

					for (int i = paramcount; i < amount - 1; ++i)
					{
						std::cout << cs[i] << pattern;
					}

					std::cout << cs[amount - 1] << std::endl;
				}
			}
		}
		catch (std::exception& ex)
		{
			std::cout << GetSystemTime().c_str() << " [" << Red << "Error" << Reset << "] " << ex.what() << std::endl;
		}
	}

	template<typename... Args>
	inline void warn(std::string content, Args... args)
	{
		try
		{
			std::lock_guard<std::mutex> guard(io_mutex);
			if (level > LogLevel::Warn) return;

			std::vector<std::string> cs = split(content, pattern);

			int paramcount = 0;
			paramcount = sizeof...(args);

			int amount = cs.size();

			if (paramcount == 0 || amount == 1)
			{
				std::cout << GetSystemTime().c_str() << " [" << Yellow << "Warn" << Reset << "] " << content << std::endl;
			}
			else
			{
				if (amount - 1 < paramcount)
				{
					std::vector<std::string> paramlist;

					Log(paramlist, args...);

					std::cout << GetSystemTime().c_str() << " [" << Yellow << "Warn" << Reset << "] ";

					for (int i = 0; i < amount - 1; ++i)
					{
						std::cout << cs[i] << paramlist[i];
					}

					std::cout << cs[amount - 1] << std::endl;
				}
				else
				{
					std::vector<std::string> paramlist;

					Log(paramlist, args...);

					std::cout << GetSystemTime().c_str() << " [" << Yellow << "Warn" << Reset << "] ";

					for (int i = 0; i < paramcount; ++i)
					{
						std::cout << cs[i] << paramlist[i];
					}

					for (int i = paramcount; i < amount - 1; ++i)
					{
						std::cout << cs[i] << pattern;
					}

					std::cout << cs[amount - 1] << std::endl;
				}
			}
		}
		catch (std::exception& ex)
		{
			std::cout << GetSystemTime().c_str() << " [" << Red << "Error" << Reset << "] " << ex.what() << std::endl;
		}
	}

	template<typename... Args>
	inline void error(std::string content, Args... args)
	{
		try
		{
			std::lock_guard<std::mutex> guard(io_mutex);
			if (level > LogLevel::Error) return;

			std::vector<std::string> cs = split(content, pattern);

			int paramcount = 0;
			paramcount = sizeof...(args);

			int amount = cs.size();

			if (paramcount == 0 || amount == 1)
			{
				std::cout << GetSystemTime().c_str() << " [" << Red << "Error" << Reset << "] " << content << std::endl;
			}
			else
			{
				if (amount - 1 < paramcount)
				{
					std::vector<std::string> paramlist;

					Log(paramlist, args...);

					std::cout << GetSystemTime().c_str() << " [" << Red << "Error" << Reset << "] ";

					for (int i = 0; i < amount - 1; ++i)
					{
						std::cout << cs[i] << paramlist[i];
					}

					std::cout << cs[amount - 1] << std::endl;
				}
				else
				{
					std::vector<std::string> paramlist;

					Log(paramlist, args...);

					std::cout << GetSystemTime().c_str() << " [" << Red << "Error" << Reset << "] ";

					for (int i = 0; i < paramcount; ++i)
					{
						std::cout << cs[i] << paramlist[i];
					}

					for (int i = paramcount; i < amount - 1; ++i)
					{
						std::cout << cs[i] << pattern;
					}

					std::cout << cs[amount - 1] << std::endl;
				}
			}
		}
		catch (std::exception& ex)
		{
			std::cout << GetSystemTime().c_str() << " [" << Red << "Error" << Reset << "] " << ex.what() << std::endl;
		}
	}
}

#endif



