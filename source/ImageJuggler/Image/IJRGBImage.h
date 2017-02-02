#pragma once 

#include "IJImage.h"

#include <array>
#include <assert.h>

// IJRGBPixel

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

// IJRGBImage

class IJRGBImage
	: public IJImage<uint8_t>
{
public:
	using PixelComp_t		= uint8_t;
	using Pixel_t			= Pixel<PixelComp_t>;
	using RGBPixel_t		= IJRGBPixel;

public:
	IJRGBImage();
	IJRGBImage(const std::string& fileName);
	IJRGBImage(const std::vector<PixelComp_t>& rawImage);
	~IJRGBImage();

	IJResult Load(std::istream& iStream) override;
	IJResult Save(std::ostream& oStream) override;

	IJResult Load(const std::vector<PixelComp_t>& rawImage);
	IJResult Save(		std::vector<PixelComp_t>& rawImage);

	using IJImage<uint8_t>::Load;
	using IJImage<uint8_t>::Save;
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
