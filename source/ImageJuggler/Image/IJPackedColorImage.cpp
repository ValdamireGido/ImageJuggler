#include "IJPackedColorImage.h"
#include "IJYCbCrImage888.h"
#include "IJRGBImage.h"
#include "IJImageTranslator.h"
#include "IJUtils.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include <fstream>
#include <strstream>
#include <assert.h>

IJPackedColorImage::IJPackedColorImage() 
	: m_yImage(nullptr)
	, m_uvImage(nullptr)
	, m_packRate(2.f)
	, m_fileName()
{}

IJPackedColorImage::IJPackedColorImage(const std::string& fileName)
	: m_yImage(nullptr)
	, m_uvImage(nullptr)
	, m_packRate(2.f)
	, m_fileName(fileName)
{}

IJPackedColorImage::IJPackedColorImage(const std::string& fileName, float packRate)
	: m_yImage(nullptr)
	, m_uvImage(nullptr)
	, m_packRate(packRate)
	, m_fileName(fileName)
{}

IJPackedColorImage::~IJPackedColorImage() 
{
	if (m_yImage)
	{
		delete m_yImage;
		m_yImage = nullptr;
	}

	if (m_uvImage)
	{
		delete m_uvImage;
		m_uvImage = nullptr;
	}
}

IJResult IJPackedColorImage::Load(std::istream& istream)
{
	if (!m_yImage)
	{
		CreateCompImages();
	}

	IJResult result = IJResult::UnknownResult;
	result = m_yImage->Load(istream);
	assert(result == IJResult::Success);

	result = m_uvImage->Load(istream);
	assert(result == IJResult::Success);

	return result;
}

IJResult IJPackedColorImage::Save(std::ostream& ostream)
{
	ASSERT_PTR(m_yImage);
	ASSERT_PTR(m_uvImage);

	IJResult result = IJResult::UnknownResult;
	result = m_yImage->Save(ostream);
	assert(result == IJResult::Success);

	result = m_uvImage->Save(ostream);
	assert(result == IJResult::Success);

	return result;
}

IJResult IJPackedColorImage::PackImage(IJYCbCrImage888* image, float rate)
{
	ASSERT_PTR(image);
	
	if (!m_yImage)
	{
		CreateCompImages();
	}

	if (rate != GetPackRate())
	{
		SetPackRate(rate);
	}

	size_t ySize = image->GetSize();

	std::vector<uint8_t> uvComps, yComp;
	yComp.resize(ySize);
	uvComps.resize(ySize*2); // double size for two comps
	for (size_t i = 0, j = 0; i < ySize; i++)
	{
		auto& pixel = (*image->GetPixelData()[i]);
		yComp[i] = pixel[0];
		uvComps[j] = pixel[1];
		++j;
		uvComps[j] = pixel[2];
		++j;
	}

	{
		std::vector<uint8_t> tempVec;
		tempVec.resize(uvComps.size());
		stbir__resize_arbitrary(nullptr,
								(const void*)&uvComps.front(), image->GetWidth(), image->GetHeight(), 0,
								(void*)&tempVec.front(), 
								static_cast<int>(image->GetWidth() / m_packRate), static_cast<int>(image->GetHeight() / m_packRate), 0,
								0, 0, 1, 1, nullptr, 2, -1, 0,
								STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
								STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);
		std::swap(uvComps, tempVec);
		assert(uvComps.size());
	}

	IJResult result = IJResult::UnknownResult;

	std::strstream yStream((char*)&yComp.front(), yComp.size(), std::ios::in);
	result = m_yImage->Load(yStream);
	assert(result == IJResult::Success);

	std::strstream uvStream((char*)&uvComps.front(), uvComps.size(), std::ios::in);
	result = m_uvImage->Load(uvStream);
	assert(result == IJResult::Success);

	return result;
}

IJResult IJPackedColorImage::UnpackImage(IJYCbCrImage888* image, float rate)
{
	ASSERT_PTR(image);
	ASSERT_PTR(m_yImage);
	ASSERT_PTR(m_uvImage);
	assert(m_yImage->GetSize());
	assert(m_uvImage->GetSize());

	std::vector<uint8_t> rawImage;
	for (size_t i = 0, size = m_yImage->GetSize(); i < size; i++)
	{
		auto& yPixel = *m_yImage->GetPixelData()[i];
		size_t uvIdx = static_cast<size_t>(i*m_packRate);
		auto& uPixel = *m_uvImage->GetPixelData()[uvIdx]; // taking the pixel data with offset derived from the pack rate of the resul image
		auto& vPixel = *m_uvImage->GetPixelData()[uvIdx+1];

		rawImage.push_back(yPixel[0]);
		rawImage.push_back(uPixel[0]);
		rawImage.push_back(vPixel[0]);
	}

	return image->Load(rawImage);
}

IJResult IJPackedColorImage::PackRGBImabe(IJRGBImage* image, float rate)
{
	ASSERT_PTR(image);

	if (!m_yImage)
	{
		CreateCompImages();
	}

	if (rate != GetPackRate())
	{
		SetPackRate(rate);
	}

	IJResult result = IJResult::UnknownResult;

	IJYCbCrImage888* yuvImage = new IJYCbCrImage888();
	result = IJImageTranslator::RGBToYCbCr888(image, yuvImage);
	if (result != IJResult::Success)
	{
		return result;
	}

	result = PackImage(yuvImage, m_packRate);
	return result;
}

IJResult IJPackedColorImage::UnpackRGBImage(IJRGBImage* image, float rate)
{
	ASSERT_PTR(image);
	ASSERT_PTR(m_yImage);
	ASSERT_PTR(m_uvImage);
	assert(m_yImage->GetSize());
	assert(m_uvImage->GetSize());

	if (rate != GetPackRate())
	{
		SetPackRate(rate);
	}

	IJYCbCrImage888* yuvImage = new IJYCbCrImage888();
	ASSERT_PTR(yuvImage);

	IJResult result = IJResult::UnknownResult;

	result = UnpackImage(yuvImage, m_packRate);
	if (result != IJResult::Success)
	{
		return result;
	}

	result = IJImageTranslator::YCbCr888ToRGB(yuvImage, image);
	return result;
}


/*
		Private methods realization
*/

void IJPackedColorImage::CreateCompImages()
{
	ASSERT_PTR_VOID(!m_yImage);
	ASSERT_PTR_VOID(!m_uvImage);

	m_yImage = new IJSingleCompImage();
	assert(m_yImage);

	m_uvImage = new IJSingleCompImage();
	assert(m_uvImage);
}

IJResult IJPackedColorImage::Load()
{
	assert(m_fileName.size());
	if (m_fileName.empty())
	{
		return IJResult::InvalidFileName;
	}

	std::ifstream ifile;
	ifile.open(m_fileName, std::ios::in);
	if (!ifile.is_open())
	{
		return IJResult::UnableToOpenFile;
	}

	return Load(ifile);
}

IJResult IJPackedColorImage::Save()
{
	assert(m_fileName.size());
	if (m_fileName.empty())
	{
		return IJResult::InvalidFileName;
	}

	std::ofstream ofile(m_fileName, std::ios::out | std::ios::binary);
	return Save(ofile);
}
