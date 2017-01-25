#pragma once 

#include "IJImage.h"

#include <array>
#include <assert.h>

class IJRGBImage
	: public IJImage
{
public:
	struct IJRGBPixel
		: public IJImage::Pixel
	{
		static constexpr uint32_t k_compCount = 3u;
		using CompData = std::array<uint8_t, static_cast<size_t>(k_compCount)>;

		IJRGBPixel() = default;
		IJRGBPixel(const std::vector<uint8_t>& _data)
		{
			assert(_data.size() == k_compCount);
			std::copy(_data.begin(), _data.end(), data.begin());
		}

		IJRGBPixel(const CompData& _data)
		{
			std::copy(_data.begin(), _data.end(), data.begin());
		}

		IJRGBPixel(IJRGBPixel&) = delete;
		IJRGBPixel(const IJRGBPixel&& other)
		{
			std::copy(other.data.begin(), other.data.end(), data.begin());
		}

		IJRGBPixel& operator=(IJRGBPixel&) = delete;
		IJRGBPixel& operator=(const IJRGBPixel&& other)
		{
			std::copy(other.data.begin(), other.data.end(), data.begin());
		}

		float operator[](uint32_t compIdx) const override
		{
			return data[compIdx];
		}

		uint32_t compCount() const override
		{
			return k_compCount;
		}

		/** 3 component for pixel: 
			R - 0, 
			G - 1, 
			B - 2 
			8 bits per component*/
		CompData data;
	};

public:
	IJRGBImage() = default;

	IJResult LoadRGB(const std::string& fileName);
};