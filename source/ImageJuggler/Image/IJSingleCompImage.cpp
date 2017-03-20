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

IJResult IJSingleCompImage::Load(std::istream& istream, size_t size)
{
	SetSize(size);
	for (size_t i = 0; i < size; i++)
	{
		Pixel_t::Comp_t comp = 0;
		istream.read((char*)&comp, Pixel_t::k_compCount);
		IJSingleCompPixel* pixel = new IJSingleCompPixel(comp);
		assert(pixel);
		AddPixel(pixel);
	}

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

IJResult IJSingleCompImage::DebugSave(const std::string& fileName, uint8_t compsToDump)
{
	std::ofstream ofile(fileName, std::ios::out | std::ios::binary);
	SaveHeader(ofile);
	for (size_t i = 0, size = GetSize(); i < size; i++)
	{
		static char zeroStr = 0;
		if ((compsToDump & B) == B)
			ofile.write((char*)&GetPixelData()[i], 1);
		else 
			ofile.write(&zeroStr, 1);

		if ((compsToDump & G) == G)
			ofile.write((char*)&GetPixelData()[i], 1);
		else 
			ofile.write(&zeroStr, 1);

		if ((compsToDump& R) == R)
			ofile.write((char*)&GetPixelData()[i], 1);
		else 
			ofile.write(&zeroStr, 1);
	}

	ofile.close();
	return IJResult::Success;
}
