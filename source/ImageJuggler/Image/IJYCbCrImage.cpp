#include "IJYCbCrImage.h"
#include <assert.h>
#include <fstream>

// Pixel Realization

IJYCbCrPixel::IJYCbCrPixel(const std::vector<IJYCbCrPixel::Comp_t>& compVector)
{
	assert(compVector.size() == IJYCbCrPixel::k_compCount);
	std::copy(compVector.begin(), compVector.end(), data.begin());
}

IJYCbCrPixel::IJYCbCrPixel(const IJYCbCrPixel::CompData_t& compVector)
{
	assert(compVector.size() == IJYCbCrPixel::k_compCount);
	std::copy(compVector.begin(), compVector.end(), data.begin());
}

IJYCbCrPixel::Comp_t IJYCbCrPixel::operator[](uint32_t compIdx) const 
{
	assert(compIdx < k_compCount);
	return data[compIdx];
}

IJYCbCrPixel::operator std::vector<IJYCbCrPixel::Comp_t>() const
{
	std::vector<Comp_t> vData;
	vData.resize(k_compCount);
	std::copy(data.begin(), data.end(), vData.begin());
	return vData;
}

// Image Realization

IJResult IJYCbCrImage::Load(const std::string& fileName) 
{
	if (fileName.empty())
	{
		return IJResult::FileNameEmpty;
	}

	SetFileName(fileName);
	return LoadYCbCr(fileName);
}

IJResult IJYCbCrImage::Save(const std::string& fileName)
{
	if (fileName.empty())
	{
		return IJResult::FileNameEmpty;
	}

	SetFileName(fileName);
	return SaveYCbCr(fileName);
}

IJResult IJYCbCrImage::LoadYCbCr(const std::string& fileName)
{
	if (fileName.empty())
	{
		return IJResult::FileNameEmpty;
	}

	SetImageType(IJImageType::YCbCr);

	std::ifstream imageFile;
	imageFile.open(fileName, std::ios::in | std::ios::binary);
	if (!imageFile.is_open())
	{
		return IJResult::UnableToOpenFile;
	}

	size_t imageSize = 0u;
	while (!imageFile.eof())
	{
		std::array<IJYCbCrPixel::Comp_t, IJYCbCrPixel::k_compCount> rawPixel;
		imageFile.read((char*)&rawPixel[0], IJYCbCrPixel::k_compCount);
		IJYCbCrPixel* pixel = new IJYCbCrPixel(rawPixel);
		assert(pixel);
		AddPixel(pixel);
		imageSize += static_cast<size_t>(IJYCbCrPixel::k_compCount);
	}

	imageFile.close();
	SetImageSize(imageSize);
	return IJResult::Success;
}

IJResult IJYCbCrImage::SaveYCbCr(const std::string& fileName)
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

	for (size_t i = 0; i < GetPixelData().size(); i++)
	{
		std::vector<IJYCbCrPixel::Comp_t> rawPixel(*GetPixelData()[i]);
		outputFile.write((char*)&rawPixel[0], IJYCbCrPixel::k_compCount);
	}

	outputFile.close();
	return IJResult::Success;
}
