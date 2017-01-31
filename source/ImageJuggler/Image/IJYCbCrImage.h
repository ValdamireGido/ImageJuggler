#pragma once 

#include "IJImage.h"

#include <array>

struct IJYCbCrPixel
	: public Pixel<uint8_t>
{
	static constexpr uint32_t k_compCount = 3u;
	using Comp_t		= uint8_t;
	// CompData_t will held Y chanal in higher word and Cb and Cr in lower word
	using CompData_t	= uint8_t;

	IJYCbCrPixel() = default;
	IJYCbCrPixel(const IJYCbCrPixel&  other);
	IJYCbCrPixel(	   IJYCbCrPixel&& other) = delete;
	IJYCbCrPixel& operator=(const IJYCbCrPixel&  other);
	IJYCbCrPixel& operator=(	  IJYCbCrPixel&& other) = delete;

	IJYCbCrPixel(const std::vector<Comp_t>& data);
	IJYCbCrPixel(const CompData_t& data);

	Comp_t		operator[](uint32_t compIdx)	const override;
	uint32_t	compCount()						const override;

	/** 3 components
		Y  - 0 (4 bits)
		Cb - 1 (2 bits)
		Cr - 2 (2 bits)
	*/
	CompData_t data;
};

class YCbCrImage
	: public IJImage<uint8_t>
{

};