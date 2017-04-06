#pragma once

#include "IJTypes.h"

#include <functional>
#include <thread>
#include <future>
#include <chrono>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include <cstdarg>

#if IJUTILS_ENABLE_SPECIAL_LOGGER
#include <fstream>
#include <iostream>
#include <iomanip>
#endif

#define SYSTEM_CLOCK_NOW_MCS() std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock().now().time_since_epoch())
#define SYSTEM_CLOCK_NOW_MS()  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock().now().time_since_epoch())
#define SYSTEM_CLOCK_NOW_S()   std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock().now().time_since_epoch())
#define SYSTEM_CLOCK_NOW_h()   std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock().now().time_since_epoch())


#define ASSERT_PTR(ptr)\
do {\
	assert(ptr);\
	if (!ptr) {\
		return IJResult::BadMemoryPointer;\
	}\
}while(false)

#define ASSERT_PTR_VOID(ptr)\
do {\
	assert(ptr);\
	if (!ptr) {\
		return;\
	}\
} while (false)

#define ASSERT_PTR_INT(ptr)\
do {\
	assert(ptr);\
	if (!ptr) {\
		return 3;\
	}\
}while(false)

namespace cusmath
{
	template <typename T>
	T clamp(const T& val, const T& lower, const T& upper)
	{
		if (val < lower)
		{
			return lower;
		}
		else if (val > upper) 
		{
			return upper;
		}
		else
			return val;
	}
}

namespace parallel
{
	inline void asyncForeach(int _start,
							 int _end,
							 std::function<void(int _i)> _func, 
							 const size_t _nThreads = 4)
	{
		if (!_func)
		{
			return;
		}

		if (_start == _end)
		{
			return;
		}

		std::vector<std::future<void> > handles;
		handles.resize(_nThreads);
		int size = _end - _start;
		int sizePerThread = size / _nThreads;
		for (int handleIdx = 0; handleIdx < _nThreads; handleIdx++)
		{
			int offset = sizePerThread * handleIdx;
			int start = _start + offset;
			int end = start + sizePerThread;
			handles[handleIdx] = std::async(std::launch::async, 
											[start, end, &_func] () 
			{
				for (int i = start; i < end; i++)
				{
					_func(i);
				}
			});
		}

		for (auto& handle : handles)
		{
			handle.wait();
		}
	}
}

#if IJUTILS_ENABLE_SPECIAL_LOGGER
namespace special_logger
{
	std::string PrettyNowTime()
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];
			
		time (&rawtime);
		timeinfo = localtime(&rawtime);
			
		strftime(buffer,sizeof(buffer),"%d-%m-%Y %I:%M:%S",timeinfo);
		std::string str(buffer);
		return str;
	}

	std::string Get_VA_ARGS(const char* format, ...)
	{
		char dest[1024 * 16];
		va_list argptr;
		va_start(argptr, format);
		vsprintf(dest, format, argptr);
		va_end(argptr);
		return std::string(dest);
	}

	class SpecialLogInterface
	{
	public: 
		SpecialLogInterface() = delete;
		inline SpecialLogInterface(std::string message, std::ostream& ostream = std::cout)
			: m_message(message)
			, m_ostream(ostream)
		{}

	protected:
		inline virtual void _PrintSummary() = 0;

		std::ostream& m_ostream;
		std::string m_message;
	};

	class SpecialLog 
		: public SpecialLogInterface
	{
	public:
		SpecialLog() = delete;

		inline SpecialLog(std::string message, std::ostream& oStream = std::cout)
			: SpecialLogInterface(message, oStream)
		{
			_PrintSummary();
		}

	protected:
		inline void _PrintSummary() override
		{
			m_ostream << std::left << "[" << PrettyNowTime() << "] - " << m_message.c_str() << std::endl;
		}

	};

	class SpecialLog_scoped
		: public SpecialLogInterface
	{
	public:
		SpecialLog_scoped() = delete;

		inline SpecialLog_scoped(std::string message, std::ostream& ostream = std::cout)
			: SpecialLogInterface(message, ostream)
		{
			m_startTime = SYSTEM_CLOCK_NOW_MS();
		}

		inline ~SpecialLog_scoped()
		{
			_PrintSummary();
		}

	protected:
		inline void _PrintSummary() override 
		{
			m_endTime = SYSTEM_CLOCK_NOW_MS();
			std::chrono::milliseconds time = m_endTime - m_startTime;
			m_ostream << std::left << "[" << PrettyNowTime() << "] Duration: " << time.count() << " - " << m_message.c_str() << std::endl;
		}

	private:
		std::chrono::milliseconds m_startTime;
		std::chrono::milliseconds m_endTime;
	};
}

#define IJ_SPECIAL_LOG(msg, ...) special_logger::SpecialLog(special_logger::Get_VA_ARGS(msg, __VA_ARGS__))
#define IJ_SPECIAL_LOG_FILE(ofstream_file, msg, ...) special_logger::SpecialLog(special_logger::Get_VA_ARGS(msg, __VA_ARGS__), ofstream_file)

#define IJ_SPECIAL_LOG_SCOPE_DURATION(msg, ...) special_logger::SpecialLog_scoped __special_log_scoped_duration_var_for_log(special_logger::Get_VA_ARGS(msg, __VA_ARGS__))
#define IJ_SPECIAL_LOG_SCOPE_DURATION_FILE(ofstream_file, msg, ...) special_logger::SpecialLog_scoped __special_log_scoped_duration_var_for_log_file(special_logger::Get_VA_ARGS(msg, __VA_ARGS__), ofstream_file)
#else
#define IJ_SPECIAL_LOG(msg, ...)
#define IJ_SPECIAL_LOG_FILE(ofstream_file, msg, ...)

#define IJ_SPECIAL_LOG_SCOPE_DURATION(msg, ...)
#define IJ_SPECIAL_LOG_SCOPE_DURATION_FILE(ofstream_file, msg, ...)
#endif
