#include "IJYCbCrImage444.h"
#include <assert.h>
#include <fstream>
#include <sstream>

// Pixel Realization

IJYCbCrPixel444::IJYCbCrPixel444(const std::vector<IJYCbCrPixelComp_t>& compVector)
{
	assert(compVector.size() == IJYCbCrPixel444::k_compCount);
	std::copy(compVector.begin(), compVector.end(), data.begin());
}

IJYCbCrPixel444::IJYCbCrPixel444(const IJYCbCrPixel444::CompData_t& compVector)
{
	assert(compVector.size() == IJYCbCrPixel444::k_compCount);
	std::copy(compVector.begin(), compVector.end(), data.begin());
}

IJYCbCrPixelComp_t& IJYCbCrPixel444::operator[](uint32_t compIdx)
{
	assert(compIdx < k_compCount);
	return data[compIdx];
}

IJYCbCrPixel444::operator std::vector<IJYCbCrPixelComp_t>() const
{
	std::vector<Comp_t> vData;
	vData.resize(k_compCount);
	std::copy(data.begin(), data.end(), vData.begin());
	return vData;
}

// Image Realization

IJYCbCrImage444::IJYCbCrImage444()
	: IJImage<IJYCbCrPixelComp_t>(IJImageType::YCbCr444)
{}

IJYCbCrImage444::IJYCbCrImage444(const std::string& fileName)
	: IJImage<IJYCbCrPixelComp_t>(fileName, IJImageType::YCbCr444)
{}

IJYCbCrImage444::IJYCbCrImage444(const std::vector<IJYCbCrPixelComp_t>& rawImage)
	: IJImage<IJYCbCrPixelComp_t>(IJImageType::YCbCr444)
{
	SetSize(rawImage.size());
	IJResult result = Load(rawImage);
	assert(result == IJResult::Success);
}

IJYCbCrImage444::~IJYCbCrImage444()
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

IJResult IJYCbCrImage444::Load(std::istream& iStream)
{
	size_t imageSize = 0u;
	while (!iStream.eof())
	{
		std::array<IJYCbCrPixel444::Comp_t, IJYCbCrPixel444::k_compCount> rawPixel;
		iStream.read((char*)&rawPixel[0], IJYCbCrPixel444::k_compCount);
		IJYCbCrPixel444* pixel = new IJYCbCrPixel444(rawPixel);
		assert(pixel);
		AddPixel(pixel);
		imageSize += static_cast<size_t>(IJYCbCrPixel444::k_compCount);
	}

	SetSize(imageSize);
	return IJResult::Success;
}

IJResult IJYCbCrImage444::Save(std::ostream& oStream)
{
	for (size_t i = 0; i < GetPixelData().size(); i++)
	{
		std::vector<IJYCbCrPixel444::Comp_t> rawPixel(*GetPixelData()[i]);
		oStream.write((char*)&rawPixel[0], IJYCbCrPixel444::k_compCount);
	}

	return IJResult::Success;
}

IJResult IJYCbCrImage444::Load(const std::vector<IJYCbCrPixelComp_t>& rawData)
{
	assert(!rawData.empty());
	assert(!GetPixelData().empty());

	std::stringstream iStream(std::string(rawData.begin(), rawData.end()));
	return Load(iStream);
}

IJResult IJYCbCrImage444::Save(std::vector<IJYCbCrPixelComp_t>& rawData)
{
	for (size_t i = 0; i < GetPixelData().size(); i++)
	{
		std::vector<IJYCbCrPixelComp_t>& rawPixel = static_cast<std::vector<IJYCbCrPixelComp_t> >(*GetPixelData()[i]);
		rawData.insert(rawData.end(), rawPixel.begin(), rawPixel.end());
	}

	return IJResult::Success;
}
