#include "IJRGBImage.h"

#include <fstream>
#include <vector>

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

IJResult IJRGBImage::Load(const std::string& fileName) 
{
	return LoadRGB(fileName);
}

IJResult IJRGBImage::Save(const std::string& fileName)
{
	return SaveRGB(fileName);
}

IJResult IJRGBImage::LoadRGB(const std::string& fileName)
{
	SetImageType(IJImageType::RGB);
	SetFileName(fileName);

	std::ifstream imageFile;
	imageFile.open(fileName, std::ios::in | std::ios::binary);
	if (!imageFile.is_open())
	{
		return IJResult::UnableToOpenFile;
	}

	size_t imageSize = 0u;
	while (!imageFile.eof())
	{
		std::array<IJRGBPixel::Comp_t, IJRGBPixel::k_compCount> rawPixel;
		imageFile.read((char*)&rawPixel[0], IJRGBPixel::k_compCount);
		IJRGBPixel* pixel = new IJRGBPixel(rawPixel);
		assert(pixel);
		AddPixel(pixel);
		imageSize += static_cast<size_t>(IJRGBPixel::k_compCount);
	}

	imageFile.close();
	SetImageSize(imageSize);

	return IJResult::Success;
}

IJResult IJRGBImage::SaveRGB(const std::string& fileName)
{
	if (fileName.empty())
	{
		return IJResult::FileNameEmpty;
	}

	std::ofstream outputFile;
	outputFile.open(fileName, std::ios::out | std::ios::binary);
	if (!outputFile.is_open())
	{
		return IJResult::UnableToOpenFile;
	}

	for (uint32_t i = 0; i < GetPixelData().size(); i++)
	{
		std::vector<IJRGBPixel::Comp_t> rawPixel(*GetPixelData()[i]);
		outputFile.write((char*)&rawPixel[0], IJRGBPixel::k_compCount);
	}

	outputFile.close();
	return IJResult::Success;
}