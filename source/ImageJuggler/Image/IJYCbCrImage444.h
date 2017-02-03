#pragma once 

#include "IJImage.h"
#include <array>

// Pixel definition

using IJYCbCrPixelComp_t = uint8_t;
struct IJYCbCrPixel444
	: public Pixel<IJYCbCrPixelComp_t>
{
	static constexpr uint32_t k_compCount	= 3u;

	using Comp_t	 = IJYCbCrPixelComp_t;
	using CompData_t = std::array<Comp_t, static_cast<size_t>(k_compCount)>;
	
	IJYCbCrPixel444() = default;

	IJYCbCrPixel444(IJYCbCrPixel444&& other) = delete;
	IJYCbCrPixel444& operator=(IJYCbCrPixel444&& other) = delete;

	IJYCbCrPixel444(const std::vector<Comp_t>& data);
	IJYCbCrPixel444(const CompData_t& data);

				operator std::vector<Comp_t>()	const override;
	Comp_t&		operator[](uint32_t compIdx)		  override;
	uint32_t	compCount()						const override;

	/** 3 components
		Y  - 0 (8 bits)
		Cb - 1 (8 bits)
		Cr - 2 (8 bits)
	*/
	CompData_t data;
};

// Image definition

class IJYCbCrImage444
	: public IJImage<IJYCbCrPixel444::Comp_t>
{
public:
	using PixelComp_t	= IJYCbCrPixel444::Comp_t;
	using Pixel_t		= Pixel<PixelComp_t>;
	using YCbCrPixel_t	= IJYCbCrPixel444;

public:
	IJYCbCrImage444();
	IJYCbCrImage444(const std::string& fileName);
	IJYCbCrImage444(const std::vector<PixelComp_t>& rawImage);
	~IJYCbCrImage444();

	IJResult Load(std::istream& iStream) override;
	IJResult Save(std::ostream& oStream) override;

	IJResult Load(const std::vector<PixelComp_t>& rawImage);
	IJResult Save(		std::vector<PixelComp_t>& rawImage);

	using IJImage<IJYCbCrPixelComp_t>::Load;
	using IJImage<IJYCbCrPixelComp_t>::Save;
};

// Inline realization

inline uint32_t IJYCbCrPixel444::compCount() const 
{
	return k_compCount;
}
