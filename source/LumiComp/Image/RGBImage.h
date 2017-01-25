#pragma once 

#include "LCImage.h"

#include <array>

class RGBImage
	: public LCImage
{
public:
	struct RGBPixel
		: public LCImage::Pixel
	{
		RGBPixel() = default;
		RGBPixel(const std::array<uint8_t, 3>& _data)
		{
			std::copy(_data.begin(), _data.end(), data.begin());
		}

		RGBPixel(RGBPixel&) = delete;
		RGBPixel(const RGBPixel&& other)
		{
			std::copy(other.data.begin(), other.data.end(), data.begin());
		}

		RGBPixel& operator=(RGBPixel&) = delete;
		RGBPixel& operator=(const RGBPixel&& other)
		{
			std::copy(other.data.begin(), other.data.end(), data.begin());
		}

		virtual float operator[](uint32_t compIdx) const override
		{
			return data[compIdx];
		}

		/** 3 component for pixel: 
			R - 0, 
			G - 1, 
			B - 2 
			8 bits per component*/
		std::array<uint8_t, 3> data;
	};

public:
	RGBImage() = default;

	Result LoadRGB(const std::string& fileName);
};