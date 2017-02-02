#include "IJRGBImage.h"

#include <fstream>
#include <vector>
#include <sstream>
#include <strstream>

// IJRGBPixel 

IJRGBPixel::IJRGBPixel(const std::vector<uint8_t>& _data)
{
	assert(_data.size() == k_compCount);
	std::copy(_data.begin(), _data.end(), data.begin());
}

IJRGBPixel::IJRGBPixel(const CompData_t& _data)
{
	std::copy(_data.begin(), _data.end(), data.begin());
}

IJRGBPixel::IJRGBPixel(const IJRGBPixel&& other)
{
	std::copy(other.data.begin(), other.data.end(), data.begin());
}

IJRGBPixel& IJRGBPixel::operator=(const IJRGBPixel&& other)
{
	std::copy(other.data.begin(), other.data.end(), data.begin());
	return *this;
}

IJRGBPixel::operator std::vector<IJRGBPixel::Comp_t>() const 
{
	std::vector<Comp_t> vData;
	vData.resize(k_compCount);
	std::copy(data.begin(), data.end(), vData.begin());
	return vData;
}

// IJRGBImage

IJRGBImage::IJRGBImage()
	: IJImage<Pixel_t::Comp_t>(IJImageType::RGB)
{}

IJRGBImage::IJRGBImage(const std::string& fileName)
	: IJImage<Pixel_t::Comp_t>(fileName, IJImageType::RGB)
{}

IJRGBImage::IJRGBImage(const std::vector<PixelComp_t>& rawImage)
	: IJImage<Pixel_t::Comp_t>(IJImageType::RGB)
{
	SetImageSize(rawImage.size());
	IJResult result = Load(rawImage);
	assert(result == IJResult::Success);
}

IJRGBImage::~IJRGBImage()
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

IJResult IJRGBImage::Load(std::istream& iStream)
{
	size_t imageSize = 0u;
	while (!iStream.eof())
	{
		std::array<IJRGBPixel::Comp_t, IJRGBPixel::k_compCount> rawPixel;
		iStream.read((char*)&rawPixel[0], IJRGBPixel::k_compCount);
		IJRGBPixel* pixel = new IJRGBPixel(rawPixel);
		assert(pixel);
		AddPixel(pixel);
		imageSize += static_cast<size_t>(IJRGBPixel::k_compCount);
	}

	SetImageSize(imageSize);
	return IJResult::Success;
}

IJResult IJRGBImage::Save(std::ostream& oStream)
{
	for (uint32_t i = 0; i < GetPixelData().size(); i++)
	{
		std::vector<IJRGBPixel::Comp_t> rawPixel(*GetPixelData()[i]);
		oStream.write((char*)&rawPixel[0], IJRGBPixel::k_compCount);
	}

	return IJResult::Success;
}

IJResult IJRGBImage::Load(const std::vector<PixelComp_t>& rawImage)
{
	assert(!rawImage.empty());
	assert(GetPixelData().empty());
	
	struct omembuf
	    : std::streambuf
	{
		omembuf(char* base, std::size_t size) {
			this->setp(base, base + size);
		}
		char* begin() const { return this->pbase(); }
		char* end() const { return this->pptr(); }
	};

	std::strstream stream((char*)&rawImage[0], rawImage.size(), std::ios::in);
	return Load(stream);
/*
	std::vector<PixelComp_t>::const_iterator it = rawImage.begin();
	while (it != rawImage.end())
	{
		std::array<IJRGBPixel::Comp_t, IJRGBPixel::k_compCount> rawPixel;
		std::copy(it, it + IJRGBPixel::k_compCount, rawPixel.begin());
		IJRGBPixel* pixel = new IJRGBPixel(rawPixel);
		assert(pixel);
		AddPixel(pixel);
	}

	return IJResult::Success;*/
}

IJResult IJRGBImage::Save(std::vector<PixelComp_t>& rawImage)
{
	assert(!GetPixelData().empty());
	auto it = GetPixelData().begin();
	while (it != GetPixelData().end())
	{
		std::vector<PixelComp_t> rawPixel = static_cast<std::vector<PixelComp_t> >(*(*it));
		rawImage.insert(rawImage.end(), rawPixel.begin(), rawPixel.end());
		it++;
	}

	return IJResult::Success;
}
