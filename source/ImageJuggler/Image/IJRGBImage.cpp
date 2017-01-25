#include "IJRGBImage.h"

#include <fstream>
#include <vector>

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

	std::vector<uint8_t> imageData;
	{
		std::string data;
		while (!imageFile.eof())
		{
			imageFile >> data;
		}

		imageFile.close();
		if (data.empty())
		{
			return IJResult::UnableToLoadFile;
		}

		imageData.resize(data.size());
		std::copy(data.begin(), data.end(), imageData.begin());
	}

	std::vector<uint8_t>::const_iterator it = imageData.begin();
	for (; it != imageData.end(); it += IJRGBPixel::k_compCount)
	{
		std::array<uint8_t, IJRGBPixel::k_compCount> rawPixel;
		std::copy(it, it + IJRGBPixel::k_compCount, rawPixel.begin());
		IJRGBPixel* pixel = new IJRGBPixel(rawPixel);
		assert(pixel);
		AddPixel(pixel);
	}

	SetImageSize(imageData.size());

	return IJResult::Success;
}