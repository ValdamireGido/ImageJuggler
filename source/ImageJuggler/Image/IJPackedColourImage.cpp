#include "IJPackedColourImage.h"
#include "IJYCbCrImage888.h"
#include "IJRGBImage.h"
#include "IJImageTranslator.h"
#include "IJUtils.h"

#define PROFILING_ENABLED 1
#include "../../../prj/Test/Profiling.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include <fstream>
#include <strstream>
#include <assert.h>

#define PACKED_COLOR_ALGORITHM_USING_STBIR 1

IJPackedColourImage::IJPackedColourImage() 
	: m_yImage(nullptr)
	, m_uImage(nullptr)
	, m_vImage(nullptr)
	, m_packRate(2)
	, m_fileName()
{}

IJPackedColourImage::IJPackedColourImage(const std::string& fileName)
	: m_yImage(nullptr)
	, m_uImage(nullptr)
	, m_vImage(nullptr)
	, m_packRate(2)
	, m_fileName(fileName)
{}

IJPackedColourImage::IJPackedColourImage(const std::string& fileName, uint8_t packRate)
	: m_yImage(nullptr)
	, m_uImage(nullptr)
	, m_vImage(nullptr)
	, m_packRate(packRate)
	, m_fileName(fileName)
{}

IJPackedColourImage::~IJPackedColourImage() 
{
	if (m_yImage)
	{
		delete m_yImage;
		m_yImage = nullptr;
	}

	if (m_uImage)
	{
		delete m_uImage;
		m_uImage = nullptr;
	}

	if (m_vImage)
	{
		delete m_vImage;
		m_vImage = nullptr;
	}
}

IJResult IJPackedColourImage::Load(std::istream& istream)
{
	if (!m_yImage)
	{
		CreateCompImages();
	}

	if (!m_header)
	{
		m_header = new TGAHeader();
		ASSERT_PTR(m_header);
	}

	istream.read((char*)&m_ySize, 4);
	istream.read((char*)&m_uvSize, 4);

	m_header->deserialize(istream);

	IJResult result = IJResult::UnknownResult;
	result = m_yImage->Load(istream, m_ySize);
	assert(result == IJResult::Success);

	result = m_uImage->Load(istream, m_uvSize);
	assert(result == IJResult::Success);

	result = m_vImage->Load(istream, m_uvSize);
	assert(result == IJResult::Success);

	return result;
}

IJResult IJPackedColourImage::Save(std::ostream& ostream)
{
	ASSERT_PTR(m_yImage);
	ASSERT_PTR(m_uImage);
	ASSERT_PTR(m_vImage);

	ostream.write((const char*)&m_ySize, 4);
	ostream.write((const char*)&m_uvSize, 4);

	m_header->serialize(ostream);

	IJResult result = IJResult::UnknownResult;
	result = m_yImage->Save(ostream);
	assert(result == IJResult::Success);

	result = m_uImage->Save(ostream);
	assert(result == IJResult::Success);

	result = m_vImage->Save(ostream);
	assert(result == IJResult::Success);
	
	return result;
}

IJResult IJPackedColourImage::PackImage(IJYCbCrImage888* image, uint8_t rate)
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

	LoadHeader(image);

	size_t size = image->GetWidth() * image->GetHeight();

	std::vector<uint8_t> yComp, uComp, vComp;
	yComp.resize(size);
	uComp.resize(size);
	vComp.resize(size);

	for (size_t i = 0; i < size; i++)
	{
		auto& pixel = image->GetData()[i];
		yComp[i] = pixel[0];
		uComp[i] = pixel[1];
		vComp[i] = pixel[2];
	}

#if defined(_DEBUG)
	image->Save("output/dbg/debug_yuv_image.tga");
#endif

	{
		dbg__profileBlock2("Comps resize: Separate components resize. ");

		std::vector<uint8_t> tempVec;
		tempVec.resize(uComp.size());
		float outputSizeW = (float)image->GetWidth() / m_packRate;
		float outputSizeH = (float)image->GetHeight() / m_packRate;
		size_t outputSize = static_cast<size_t>(outputSizeW * outputSizeH) + 1;

		stbir__resize_arbitrary(nullptr,
								(const void*)&uComp.front(), image->GetWidth(), image->GetHeight(), 0,
								(void*)&tempVec.front(), (int)outputSizeW, (int)outputSizeH, 0,
								0, 0, 1, 1, nullptr, 1, -1, 0,
								STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
								STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);

		std::swap(uComp, tempVec);
		assert(uComp.size());

		stbir__resize_arbitrary(nullptr,
			(const void*)&vComp.front(), image->GetWidth(), image->GetHeight(), 0,
								(void*)&tempVec.front(), (int)outputSizeW, (int)outputSizeH, 0,
								0, 0, 1, 1, nullptr, 1, -1, 0,
								STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
								STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);

		std::swap(vComp, tempVec);
		assert(vComp.size());

		uComp.resize(outputSize);
		vComp.resize(outputSize);
		m_uvSize = uComp.size();
	}

	IJResult result = IJResult::UnknownResult;
	m_ySize = yComp.size();

	std::strstream yStream((char*)&yComp.front(), yComp.size(), std::ios::in | std::ios::binary);
	result = m_yImage->Load(yStream, yComp.size());
	assert(result == IJResult::Success);

	std::strstream uvStream((char*)&uComp.front(), uComp.size(), std::ios::in | std::ios::binary);
	result = m_uImage->Load(uvStream, uComp.size());
	assert(result == IJResult::Success);

	std::strstream vStream((char*)&vComp.front(), vComp.size(), std::ios::in | std::ios::binary);
	result = m_vImage->Load(vStream, vComp.size());
	assert(result == IJResult::Success);

	return result;
}

IJResult IJPackedColourImage::UnpackImage(IJYCbCrImage888* image, uint8_t rate)
{
	ASSERT_PTR(image);
	ASSERT_PTR(m_yImage);
	ASSERT_PTR(m_uImage);
	ASSERT_PTR(m_vImage);
	assert(m_yImage->GetSize());
	assert(m_uImage->GetSize());
	assert(m_vImage->GetSize());

	std::vector<uint8_t> rawImage;
	auto& Y = *m_yImage;
	auto& U = *m_uImage;
	auto& V = *m_vImage;
	size_t W = static_cast<size_t>(m_header->width);
	size_t H = static_cast<size_t>(m_header->height);
	size_t packedW = static_cast<size_t>(m_header->width / m_packRate);

#define PACKED_COLOUR_IMAGE_UNPACK_WAY_TEST_1 0
#define PACKED_COLOUR_IMAGE_UNPACK_WAY_TEST_2 0
#define PACKED_COLOUR_IMAGE_UNPACK_WAY_TEST_3 0
#define PACKED_COLOUR_IMAGE_UNPACK_WAY_TEST_4 1
#if PACKED_COLOUR_IMAGE_UNPACK_WAY_TEST_1
	for (size_t j = 0, uvj = 0; j < H; j++)
	{
		if (j % m_packRate == 0	&& j != 0)
		{
			++uvj;
		}

		for (size_t i = 0, uvi = 0, uvidx = 0; i < W; i++)
		{
			if (i % m_packRate == 0	&& i != 0)
			{
				++uvi;
			}

			uvidx = uvi + packedW * uvj;
			rawImage.push_back(Y[i*j]);
			rawImage.push_back(U[uvidx]);
			rawImage.push_back(V[uvidx]);
		}
	}
#elif PACKED_COLOUR_IMAGE_UNPACK_WAY_TEST_2
	for (size_t i = 0, uvi = 0; i < Y.GetSize(); i++)
	{
		uvi = i / (m_packRate*m_packRate);
		
		rawImage.push_back(Y[i]);
		rawImage.push_back(U[uvi]);
		rawImage.push_back(V[uvi]);
	}
#elif PACKED_COLOUR_IMAGE_UNPACK_WAY_TEST_3
	for (size_t i = 0, 
		 j = 0, 
		 uvi = 0, 
		 uvidx = 0; i < Y.GetSize(); i++)
	{
		if (i % (W*2) == 0)
		{
			++j;
			uvi = 0;
		}

		uvidx = (uvi/m_packRate) + ((j-1)*packedW);

		rawImage.push_back(Y[i]);
		rawImage.push_back(U[uvi]);
		rawImage.push_back(V[uvi]);

		++uvi;
	}
#elif PACKED_COLOUR_IMAGE_UNPACK_WAY_TEST_4
	std::vector<uint8_t> uC;
	std::vector<uint8_t> vC;
	uC.resize(Y.GetSize());
	vC.resize(Y.GetSize());

	{
		dbg__profileBlock2("Comps resize upsameple");

		stbir__resize_arbitrary(nullptr, 
								(const void*)&U[0], packedW, packedW, 0, 
								(void*)&uC.front(), W, H, 0, 
								0, 0, 1, 1, nullptr, 1, -1, 0,
								STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
								STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);

		stbir__resize_arbitrary(nullptr, 
								(const void*)&V[0], packedW, packedW, 0, 
								(void*)&vC.front(), W, H, 0, 
								0, 0, 1, 1, nullptr, 1, -1, 0,
								STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
								STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);
	}

	for (size_t i = 0; i < Y.GetSize(); i++)
	{
		rawImage.push_back(Y[i]);
		rawImage.push_back(uC[i]);
		rawImage.push_back(vC[i]);
	}
#endif

	SaveHeader(image);
	return image->Load(rawImage);
}

IJResult IJPackedColourImage::PackRGBImage(IJRGBImage* image, uint8_t rate)
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

	IJYCbCrImage888* yuvImage = nullptr;
	IJResult result = IJResult::UnknownResult;
	{
		dbg__profileBlock2("RGB -> YUV translation");
		yuvImage = new IJYCbCrImage888();
		result = IJImageTranslator::RGBToYCbCr888(image, yuvImage);
		if (result != IJResult::Success)
		{
			return result;
		}
	}

	{
		dbg__profileBlock2("YUV comp pack");
		result = PackImage(yuvImage, m_packRate);
	}

	if (yuvImage)
	{
		delete yuvImage;
		yuvImage = nullptr;
	}
	return result;
}

IJResult IJPackedColourImage::UnpackRGBImage(IJRGBImage* image, uint8_t rate)
{
	ASSERT_PTR(image);
	ASSERT_PTR(m_yImage);
	ASSERT_PTR(m_uImage);
	ASSERT_PTR(m_vImage);
	assert(m_yImage->GetSize());
	assert(m_uImage->GetSize());
	assert(m_vImage->GetSize());

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

	SaveHeader(yuvImage);
	yuvImage->Save("output/dbg/yuv_upack_dbg.tga");

	result = IJImageTranslator::YCbCr888ToRGB(yuvImage, image);

	if (yuvImage)
	{
		delete yuvImage;
		yuvImage = nullptr;
	}

	return result;
}


/*
		Private methods realization
*/

void IJPackedColourImage::CreateCompImages()
{
	ASSERT_PTR_VOID(!m_yImage);
	ASSERT_PTR_VOID(!m_uImage);
	ASSERT_PTR_VOID(!m_vImage);

	m_yImage = new IJSingleCompImage();
	assert(m_yImage);

	m_uImage = new IJSingleCompImage();
	assert(m_uImage);

	m_vImage = new IJSingleCompImage();
	assert(m_vImage);
}

IJResult IJPackedColourImage::Load()
{
	assert(m_fileName.size());
	if (m_fileName.empty())
	{
		return IJResult::InvalidFileName;
	}

	std::ifstream ifile;
	ifile.open(m_fileName, std::ios::in | std::ios::binary);
	if (!ifile.is_open())
	{
		return IJResult::UnableToOpenFile;
	}

	return Load(ifile);
}

IJResult IJPackedColourImage::Save()
{
	assert(m_fileName.size());
	if (m_fileName.empty())
	{
		return IJResult::InvalidFileName;
	}

	std::ofstream ofile(m_fileName, std::ios::out | std::ios::binary);
	return Save(ofile);
}

