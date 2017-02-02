#pragma once

#include <stdint.h>

enum class IJResult : uint8_t
{
	Success = 0, 
	UnknownError,
	MemoryAllocationError,
	BadMemoryPointer, 
	UnknownFileFormat,
	InvalidFileName,
	UnableToOpenFile, 
	UnableToLoadFile,
	UnableToSaveFile
	
};

enum class IJImageType : uint8_t
{
	RGB = 0, 
	YCbCr, 
	YCoCg
};
