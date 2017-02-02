#include "IJYCbCrImage.h"
#include <assert.h>
#include <fstream>

// Pixel Realization

IJYCbCrPixel::IJYCbCrPixel(const std::vector<IJYCbCrPixelComp_t>& compVector)
{
	assert(compVector.size() == IJYCbCrPixel::k_compCount);
	std::copy(compVector.begin(), compVector.end(), data.begin());
}

IJYCbCrPixel::IJYCbCrPixel(const IJYCbCrPixel::CompData_t& compVector)
{
	assert(compVector.size() == IJYCbCrPixel::k_compCount);
	std::copy(compVector.begin(), compVector.end(), data.begin());
}

IJYCbCrPixelComp_t IJYCbCrPixel::operator[](uint32_t compIdx) const 
{
	assert(compIdx < k_compCount);
	return data[compIdx];
}

IJYCbCrPixel::operator std::vector<IJYCbCrPixelComp_t>() const
{
	std::vector<Comp_t> vData;
	vData.resize(k_compCount);
	std::copy(data.begin(), data.end(), vData.begin());
	return vData;
}

// Image Realization

IJYCbCrImage::IJYCbCrImage()
{
	SetImageType(IJImageType::YCbCr);
	SetImageSize(0ul);
}

IJYCbCrImage::IJYCbCrImage(const std::vector<IJYCbCrPixelComp_t>& rawImage)
{
	SetImageType(IJImageType::YCbCr);
	SetImageSize(rawImage.size());
	IJResult result = Load(rawImage);
	assert(result == IJResult::Success);
}

IJYCbCrImage::~IJYCbCrImage()
{
	auto it = GetPixelData().begin();
	for ( ; it != GetPixelData().end(); it++)
	{
		Pixel_t* pixel = *it;
		if (pixel)
		{
			delete pixel;
		}
	}

	GetPixelData().clear();
}

IJResult IJYCbCrImage::Load(std::istream& iStream)
{
	size_t imageSize = 0u;
	while (!iStream.eof())
	{
		std::array<IJYCbCrPixel::Comp_t, IJYCbCrPixel::k_compCount> rawPixel;
		iStream.read((char*)&rawPixel[0], IJYCbCrPixel::k_compCount);
		IJYCbCrPixel* pixel = new IJYCbCrPixel(rawPixel);
		assert(pixel);
		AddPixel(pixel);
		imageSize += static_cast<size_t>(IJYCbCrPixel::k_compCount);
	}

	SetImageSize(imageSize);
	return IJResult::Success;
}

IJResult IJYCbCrImage::Save(std::ostream& oStream)
{
	for (size_t i = 0; i < GetPixelData().size(); i++)
	{
		std::vector<IJYCbCrPixel::Comp_t> rawPixel(*GetPixelData()[i]);
		oStream.write((char*)&rawPixel[0], IJYCbCrPixel::k_compCount);
	}

	return IJResult::Success;
}

IJResult IJYCbCrImage::Load(const std::vector<IJYCbCrPixelComp_t>& rawData)
{
	assert(!rawData.empty());
	assert(!GetPixelData().empty());
	std::vector<IJYCbCrPixelComp_t>::const_iterator it = rawData.begin();
	while (it != rawData.end())
	{
		std::array<IJYCbCrPixelComp_t, IJYCbCrPixel::k_compCount> rawPixel;
		std::copy(it, it + IJYCbCrPixel::k_compCount, rawPixel.begin());
		IJYCbCrPixel* pixel = new IJYCbCrPixel(rawPixel);
		assert(pixel);
		AddPixel(pixel);
	}

	return IJResult::Success;
}

IJResult IJYCbCrImage::Save(std::vector<IJYCbCrPixelComp_t>& rawData)
{
	for (size_t i = 0; i < GetPixelData().size(); i++)
	{
		std::vector<IJYCbCrPixelComp_t>& rawPixel = static_cast<std::vector<IJYCbCrPixelComp_t> >(*GetPixelData()[i]);
		rawData.insert(rawData.end(), rawPixel.begin(), rawPixel.end());
	}

	return IJResult::Success;
}
