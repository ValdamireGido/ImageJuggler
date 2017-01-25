#pragma once

#include <stdint.h>

enum class Result : uint8_t
{
	Success = 0, 
	UnknownError,
	UnknownFileFormat,
	
};