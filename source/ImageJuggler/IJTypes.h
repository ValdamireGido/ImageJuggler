#pragma once

#include <stdint.h>

enum class IJResult : uint8_t
{
	Success = 0, 
	UnknownError,
	UnknownFileFormat,
	UnableToOpenFile, 
	UnableToLoadFile
	
};