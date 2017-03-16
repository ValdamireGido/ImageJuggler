#pragma once 

#include "IJImageInterface.h"

/* 
		YUV Single component pixel
		Created to store every component in a single storage.
*/

using YUVPixelComp_t = uint8_t;
class IJSingleCompPixel
	: public IJPixelInterface<YUVPixelComp_t, 1u>
{
public:
	IJSingleCompPixel() = default;
	IJSingleCompPixel(Comp_t comp);
	virtual ~IJSingleCompPixel() {}

	std::vector<Comp_t>		serialize() const override;
	void					deserialize(const std::array<Comp_t, k_compCount>& rawPixel) override;
	Comp_t&					operator [] (size_t idx) override;

	Comp_t c;
};


/* 
		YUV Single Component Image
		Has three different collections for every component. 
*/
class IJSingleCompImage
	: IJImageInterface<YUVPixelComp_t, 1u>
{
public:
	using PixelData_t = std::vector<uint8_t>;

public:
	IJSingleCompImage();
	virtual ~IJSingleCompImage() {}

	IJResult Load(std::istream& iStream) override;
	IJResult Save(std::ostream& oStream) override;

	using IJImageInterface<YUVPixelComp_t, Pixel_t::k_compCount>::GetPixelData;
	using IJImageInterface<YUVPixelComp_t, Pixel_t::k_compCount>::Load;
	using IJImageInterface<YUVPixelComp_t, Pixel_t::k_compCount>::Save;
	using IJImageInterface<YUVPixelComp_t, Pixel_t::k_compCount>::GetSize;
};