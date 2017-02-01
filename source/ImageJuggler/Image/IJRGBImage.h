#pragma once 

#include "IJImage.h"

#include <array>
#include <assert.h>

struct IJRGBPixel
	: public Pixel<uint8_t>
{
	static constexpr uint32_t k_compCount = 3u;

	using Comp_t = uint8_t; 
	using CompData_t = std::array<Comp_t, static_cast<size_t>(k_compCount)>;

	IJRGBPixel() = default;
	IJRGBPixel(		 IJRGBPixel&  other) = delete;
	IJRGBPixel(const IJRGBPixel&& other );
	IJRGBPixel& operator=(		IJRGBPixel&	 other)	= delete;
	IJRGBPixel& operator=(const IJRGBPixel&& other);

	IJRGBPixel(const std::vector<Comp_t>& _data);
	IJRGBPixel(const CompData_t& _data);

				operator std::vector<Comp_t>() const override;
	Comp_t		operator[](uint32_t compIdx) const override;
	uint32_t	compCount() const override;

	/** 3 component for pixel: 
		R - 0, 
		G - 1, 
		B - 2 
		8 bits per component*/
	CompData_t data;
};

class IJRGBImage
	: public IJImage<uint8_t>
{
public:
	using PixelComp_t		= uint8_t;
	using Pixel_t			= Pixel<PixelComp_t>;
	using RGBPixel_t		= IJRGBPixel;

public:
	IJRGBImage() = default;
	IJRGBImage(const std::vector<PixelComp_t>& rawImage);

	IJResult Load(const std::string& fileName) override;
	IJResult Save(const std::string& fileName) override;
	IJResult LoadRGB(const std::string& fileName);
	IJResult SaveRGB(const std::string& fileName);
};

// Inline IJRGBPixel

inline IJRGBPixel::Comp_t IJRGBPixel::operator[](uint32_t compIdx) const 
{
	return data[compIdx];
}

inline uint32_t IJRGBPixel::compCount() const 
{
	return k_compCount;
}
