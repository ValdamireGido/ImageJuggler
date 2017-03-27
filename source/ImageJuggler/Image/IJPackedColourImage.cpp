#include "IJPackedColourImage.h"
#include "IJYCbCrImage888.h"
#include "IJRGBImage.h"
#include "IJImageTranslator.h"
#include "IJUtils.h"
#include <future>

#define PROFILING_ENABLED 1
#include "../../../prj/Test/Profiling.h"

#include "stb_image_resize.h"

#include <fstream>
#include <strstream>
#include <assert.h>

#define PACKED_COLOR_ALGORITHM_USING_STBIR 1

#define PACKED_COLOUR_IMAGE_UNPACK_ALGORITH_TEST_1 1
#define PACKED_COLOUR_IMAGE_UNPACK_ALGORITH_TEST_2 0
IJPackedColourImage::IJPackedColourImage() 
	: m_yImage(nullptr)
	, m_uImage(nullptr)
	, m_vImage(nullptr)
	, m_packRate(k_defaultPackRate)
	, m_fileName()
{}

IJPackedColourImage::IJPackedColourImage(const std::string& fileName)
	: m_yImage(nullptr)
	, m_uImage(nullptr)
	, m_vImage(nullptr)
	, m_packRate(k_defaultPackRate)
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
	m_yImage->Resize(size);
	m_uImage->Resize(size);
	m_vImage->Resize(size);
	
	std::vector<uint8_t>& yComp = m_yImage->GetData();
	std::vector<uint8_t>& uComp = m_uImage->GetData();
	std::vector<uint8_t>& vComp = m_vImage->GetData();

	//for (size_t i = 0; i < size; i++)
	std::function<void(int)> iteration = [&yComp, &uComp, &vComp, image] (int i)
	{
		auto& pixel = image->GetData()[i];
		yComp[i] = pixel.c1;
		uComp[i] = pixel.c2;
		vComp[i] = pixel.c3;
	};
	parallel::asyncForeach(0, size, iteration, k_workingThreadCount);

	{
		dbg__profileBlock2("Comps resize: Separate components resize. ");
		std::vector<uint8_t> tempVec;
		tempVec.resize(uComp.size());
		float outputSizeW = (float)image->GetWidth() / m_packRate;
		float outputSizeH = (float)image->GetHeight() / m_packRate;
		size_t outputSize = static_cast<size_t>(outputSizeW * outputSizeH) + 1;

		stbir_resize_uint8_generic((const unsigned char*)&uComp.front(), image->GetWidth(), image->GetHeight(), 0,
								   (unsigned char*)&tempVec.front(), (int)outputSizeW, (int)outputSizeH, 0,
								   1, -1, 0, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT, STBIR_COLORSPACE_LINEAR, nullptr);

		std::swap(uComp, tempVec);
		assert(uComp.size());

		stbir_resize_uint8_generic((const unsigned char*)&vComp.front(), image->GetWidth(), image->GetHeight(), 0,
								   (unsigned char*)&tempVec.front(), (int)outputSizeW, (int)outputSizeH, 0,
								   1, -1, 0, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT, STBIR_COLORSPACE_LINEAR, nullptr);

		std::swap(vComp, tempVec);
		assert(vComp.size());

		uComp.resize(outputSize);
		vComp.resize(outputSize);
		m_uvSize = uComp.size();
	}

	m_ySize = yComp.size();
	return IJResult::Success;
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

IJResult IJPackedColourImage::UnpackImage(IJYCbCrImage888* image, uint8_t rate)
{
	ASSERT_PTR(image);
	ASSERT_PTR(m_yImage);
	ASSERT_PTR(m_uImage);
	ASSERT_PTR(m_vImage);
	assert(m_yImage->GetSize());
	assert(m_uImage->GetSize());
	assert(m_vImage->GetSize());

	std::vector<IJPixel>& rawImage = image->GetData();
	auto& Y = *m_yImage;
	auto& U = *m_uImage;
	auto& V = *m_vImage;
	size_t W = static_cast<size_t>(m_header->width);
	size_t H = static_cast<size_t>(m_header->height);
	size_t packedW = static_cast<size_t>(m_header->width / m_packRate);

	std::vector<uint8_t> uC;
	std::vector<uint8_t> vC;
	uC.resize(Y.GetSize());
	vC.resize(Y.GetSize());

	{
		dbg__profileBlock2("Comps resize upsameple");
		stbir_resize_uint8_generic((const unsigned char*)&U[0], packedW, packedW, 0, 
								   (unsigned char*)&uC.front(), W, H, 0, 
								   1, -1, 0, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT, STBIR_COLORSPACE_LINEAR,
								   nullptr);

		stbir_resize_uint8_generic((const unsigned char*)&V[0], packedW, packedW, 0, 
								   (unsigned char*)&vC.front(), W, H, 0, 
								   1, -1, 0, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT, STBIR_COLORSPACE_LINEAR,
								   nullptr);
	}

	image->Resize(m_yImage->GetPixelCount());

	//for (size_t i = 0; i < Y.GetSize(); i++)
	std::function<void(int)> interation = [&rawImage, &Y, &uC, &vC] (int i)
	{
		rawImage[i].c1 = Y[i];
		rawImage[i].c2 = uC[i];
		rawImage[i].c3 = vC[i];
	};
	parallel::asyncForeach(0, image->GetPixelCount(), interation, k_workingThreadCount);

	SaveHeader(image);
	return IJResult::Success;
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

#if PACKED_COLOUR_IMAGE_UNPACK_ALGORITH_TEST_1 
	IJYCbCrImage888* yuvImage = new IJYCbCrImage888();
	ASSERT_PTR(yuvImage);
	SaveHeader(yuvImage);

	IJResult result = IJResult::UnknownResult;
	{
		dbg__profileBlock2("YUV comp unpack");
		result = UnpackImage(yuvImage, m_packRate);
		if (result != IJResult::Success)
		{
			return result;
		}
	}

	{
		dbg__profileBlock2("YUV -> RGB translation");

		result = IJImageTranslator::YCbCr888ToRGB(yuvImage, image);
	}

	if (yuvImage)
	{
		delete yuvImage;
		yuvImage = nullptr;
	}

	return result;
#elif PACKED_COLOUR_IMAGE_UNPACK_ALGORITH_TEST_2
	SaveHeader(image);
	auto& rgb = image->GetData();
	image->Resize(m_ySize);
	auto& Y = *m_yImage;
	auto& U = *m_uImage;
	auto& V = *m_vImage;
	size_t W = m_header->width;

	//for (size_t j = 0; j < W; j++)
	std::function<void(int)> interation = [&Y, &U, &V, W, &rgb, this] (int j) 
	{
		size_t uvj = (j / m_packRate) * (W / m_packRate);
		for (size_t i = 0; i < W; i++)
		{
			size_t yi = j * W + i;
			size_t uvi = uvj + (i / m_packRate);
			IJImageTranslator::TranslateYUVToRGB(Y[yi], U[uvi], V[uvi], rgb[yi].c1, rgb[yi].c2, rgb[yi].c3);
		}
	};
	parallel::asyncForeach(0, W, interation, k_workingThreadCount);

	return IJResult::Success;
#endif //
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

