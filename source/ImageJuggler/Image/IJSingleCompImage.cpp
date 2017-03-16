#include "IJSingleCompImage.h"
#include <assert.h>


/*
		YUV Single component pixel
*/

IJSingleCompPixel::IJSingleCompPixel(Comp_t comp)
	: c(comp)
{}

std::vector<YUVPixelComp_t> IJSingleCompPixel::serialize() const 
{
	return std::vector<Comp_t> { c };
}

void IJSingleCompPixel::deserialize(const std::array<Comp_t, k_compCount>& rawPixel) 
{
	assert(!rawPixel.empty());
	c = rawPixel.front();
}

inline YUVPixelComp_t& IJSingleCompPixel::operator [] (size_t idx)
{
	return c;
}


/* 
		YUV Single component image 
*/

IJSingleCompImage::IJSingleCompImage()
	: IJImageInterface<YUVPixelComp_t, 1u>(IJImageType::SinglComp)
{}

IJResult IJSingleCompImage::Load(std::istream& iStream)
{
	size_t imageSize = 0u;
	while (!iStream.eof())
	{
		Pixel_t::Comp_t comp = 0;
		iStream.read((char*)&comp, Pixel_t::k_compCount);
		IJSingleCompPixel* pixel = new IJSingleCompPixel(comp);
		assert(pixel);
		AddPixel(pixel);
		imageSize += Pixel_t::k_compCount;
	}

	SetSize(imageSize);
	return IJResult::Success;
}

IJResult IJSingleCompImage::Save(std::ostream& oStream)
{
	assert(!GetPixelData().empty());
	if (GetPixelData().empty())
	{
		return IJResult::ImageIsEmpty;
	}

	for (size_t i = 0u, size = GetPixelData().size(); i < size; i++)
	{
		std::vector<Pixel_t::Comp_t> comp = GetPixelData()[i]->serialize();
		oStream.write((char*)&comp.front(), Pixel_t::k_compCount);
	}

	return IJResult::Success;
}
