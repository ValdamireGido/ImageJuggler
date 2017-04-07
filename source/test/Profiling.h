#pragma once

#include <ostream>
#include <iostream>
#include <time.h>
#include <chrono>

#if PROFILING_ENABLED

#define dbg__profileBlock2_file(blockName, stream) \
ProfileUtils::ProfileBlock __profile__block__info__(__FILE__, __FUNCTION__, __LINE__, blockName, stream)

#define dbg__profileBlock2(blockName) \
ProfileUtils::ProfileBlock __profile__block__info__(__FILE__, __FUNCTION__, __LINE__, blockName)

#define dbg__profileBlock() \
ProfileUtils::ProfileBlock __profile__block__info__(__FILE__, __FUNCTION__, __LINE__, __FUNCTION__)

#define dbg__profielBlock_file(stream) \
ProfileUtils::ProfileBlock __profile__block__info__(__FILE__, __FUNCTION__, __LINE__, __FUNCTION__, stream)

#else 
#define dbg__profileBlock2(blockName)
#define dbg__profileBlock()
#endif


namespace ProfileUtils
{

	class ProfileBlock
	{
		static int s_level;
	public:
		ProfileBlock() = delete;
		ProfileBlock(const char* fileName, const char* funcName, int line, const char* blockName, std::ostream& oStream = std::cout);
		~ProfileBlock();

	public:
		void _PrintSummary();

	private:
		std::ostream&	m_oStream;
		std::string		m_fileName;
		std::string		m_funcName;
		int				m_line;
		std::string		m_blockName;
		std::chrono::milliseconds m_startTime;
		std::chrono::milliseconds m_endTime;
	};
}
