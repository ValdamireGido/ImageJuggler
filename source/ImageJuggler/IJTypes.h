#pragma once

#include <stdint.h>

enum class IJResult : uint8_t
{
	Success					= 0, 
	UnknownError			= 1,
	MemoryAllocationError	= 2,
	BadMemoryPointer		= 3, 
	UnknownFileFormat		= 4,
	InvalidFileName			= 5,
	UnableToOpenFile		= 6, 
	UnableToLoadFile		= 7,
	UnableToSaveFile		= 8, 
	UnableToTranslateImage	= 9, 

	ImageIsEmtpy			= 16,
	InvalidImageFormat		= 17
};

enum class IJImageType : uint8_t
{
	RGB			= 0,
	YCbCr444	= 1, 
	YCbCr422	= 2,
	YCoCg		= 3
};

enum class CompressionType : uint8_t
{
	NoImageData							= 0, 
	UncompressedColorMapped				= 1, 
	UncompressedRGB						= 2, 
	UncompressedBlackWhite				= 3, 
	RunlengthEncodedColorMapped			= 9, 
	RunlengthEncodedRGB					= 10, 
	CompressedBlackWhite				= 11, 
	CompressedColorMappedData			= 32, 
	CompressedColorMappedDataQuadtree	= 33
};