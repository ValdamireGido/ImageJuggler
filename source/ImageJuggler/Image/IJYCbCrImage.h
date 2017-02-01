#pragma once 

#include "IJImage.h"

#include <array>

// Pixel definition

using IJYCbCrPixelComp_t = uint8_t;
struct IJYCbCrPixel
	: public Pixel<IJYCbCrPixelComp_t>
{
	static constexpr uint32_t k_compCount	= 3u;
	static constexpr std::array<uint8_t, static_cast<size_t>(k_compCount)> k_compsOffset = { 0u, 4u, 6u };

	using Comp_t		= IJYCbCrPixelComp_t;
	using CompData_t	= std::array<Comp_t, static_cast<size_t>(k_compCount)>;

	IJYCbCrPixel() = default;
	IJYCbCrPixel(const IJYCbCrPixel&  other) = default;
	IJYCbCrPixel(	   IJYCbCrPixel&& other) = delete;
	IJYCbCrPixel& operator=(const IJYCbCrPixel&  other) = default;
	IJYCbCrPixel& operator=(	  IJYCbCrPixel&& other) = delete;

	IJYCbCrPixel(const std::vector<Comp_t>& data);
	IJYCbCrPixel(const CompData_t& data);

				operator std::vector<Comp_t>()	const override;
	Comp_t		operator[](uint32_t compIdx)	const override;
	uint32_t	compCount()						const override;

	/** 3 components
		Y  - 0 (8 bits)
		Cb - 1 (8 bits)
		Cr - 2 (8 bits)
	*/
	CompData_t data;
};

// Image definition

class IJYCbCrImage
	: public IJImage<IJYCbCrPixel::Comp_t>
{
public:
	using PixelComp_t		= IJYCbCrPixel::Comp_t;
	using Pixel_t			= Pixel<PixelComp_t>;
	using YCbCrPixel_t		= IJYCbCrPixel;

public:
	IJYCbCrImage() = default;
	IJYCbCrImage(const std::vector<PixelComp_t>& rawImage);

	IJResult Load(const std::string& fileName) override;
	IJResult Save(const std::string& fileName) override;
	IJResult LoadYCbCr(const std::string& fileName);
	IJResult SaveYCbCr(const std::string& fileName);
};

// Inline realization

inline uint32_t IJYCbCrPixel::compCount() const 
{
	return k_compCount;
}