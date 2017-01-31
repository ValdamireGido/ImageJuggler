#include "IJRGBImage.h"

#include <fstream>
#include <vector>

// IJRGBPixel 

IJRGBPixel::IJRGBPixel(const std::vector<uint8_t>& _data)
{
	assert(_data.size() == k_compCount);
	std::copy(_data.begin(), _data.end(), data.begin());
}

IJRGBPixel::IJRGBPixel(const CompData& _data)
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

	uint32_t imageSize = 0;
	while (!imageFile.eof())
	{
		uint8_t data[IJRGBPixel::k_compCount];
		imageFile.read((char*)data, IJRGBPixel::k_compCount);
		std::array<uint8_t, IJRGBPixel::k_compCount> rawPixel;
		rawPixel[0] = data[0];
		rawPixel[1] = data[1];
		rawPixel[2] = data[2];
		IJRGBPixel* pixel = new IJRGBPixel(rawPixel);
		assert(pixel);
		AddPixel(pixel);
		imageSize += IJRGBPixel::k_compCount;
	}

	imageFile.close();
	SetImageSize(imageSize);

	return IJResult::Success;
}

IJResult IJRGBImage::SaveRGB(const std::string& fileName)
{
	std::ofstream outputFile;
	outputFile.open(fileName, std::ios::out | std::ios::binary);
	if (!outputFile.is_open())
	{
		return IJResult::UnableToOpenFile;
	}

	std::string data;
	//data.resize(GetPixelData().size() * IJRGBPixel::k_compCount);
	for (uint32_t i = 0; i < GetPixelData().size(); i++)
	{
		const Pixel<PixelComp_t>& pixel = *GetPixelData()[i];
		data += pixel[0];
		data += pixel[1];
		data += pixel[2];
	}

	if (data.empty())
	{
		outputFile.close();
		return IJResult::UnableToSaveFile;
	}

	outputFile << data;
	outputFile.close();

	return IJResult::Success;
}