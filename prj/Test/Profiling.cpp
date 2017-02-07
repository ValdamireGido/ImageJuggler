#include "Profiling.h"

#include <iomanip>

namespace ProfileUtils
{
	int ProfileBlock::s_level = 0;

	ProfileBlock::ProfileBlock(const char* fileName, const char* funcName, int line, const char* blockName, std::ostream& oStream)
		: m_oStream(oStream)
		, m_fileName(fileName)
		, m_funcName(funcName)
		, m_line(line)
		, m_blockName(blockName)
	{
		using namespace std::chrono;
		m_startTime = duration_cast<milliseconds>(system_clock().now().time_since_epoch());
		s_level++;

		m_oStream << std::left;
		m_oStream << std::setw(4 * s_level - 1)	<< ">" << "================================================================" << std::endl;
		m_oStream << std::setw(4 * s_level - 1)	<< ">" << "Block: " << m_blockName.c_str() << std::endl;
		m_oStream << std::setw(4 * s_level - 1)	<< ">" << "{" << std::endl;
	}

	ProfileBlock::~ProfileBlock()
	{
		using namespace std::chrono;
		m_endTime = duration_cast<milliseconds>(system_clock().now().time_since_epoch());
		_PrintSummary();
		s_level--;
	}

	void ProfileBlock::_PrintSummary()
	{
		uint32_t time = (m_endTime - m_startTime).count();
		m_oStream << std::left;
		m_oStream << std::setw(4 * s_level) << ">" << std::setw(12) << "File: "			 << m_fileName.c_str()	<< std::endl
				  << std::setw(4 * s_level) << ">" << std::setw(12) << "Line: "			 << m_line				<< std::endl
				  << std::setw(4 * s_level) << ">" << std::setw(12) << "Function: "		 << m_funcName.c_str()	<< std::endl 
				  << std::setw(4 * s_level) << ">" << std::setw(16) << "Time spent(ms): " << time << std::endl;
		m_oStream << std::setw(4 * s_level - 1)	<< ">" << "}" << std::endl;
		m_oStream << std::setw(4 * s_level - 1)	<< ">" << "==== " << m_blockName.c_str() << std::endl;
		m_oStream << std::setw(4 * s_level - 1)	<< ">" << "================================================================"  << std::endl;
	}
}