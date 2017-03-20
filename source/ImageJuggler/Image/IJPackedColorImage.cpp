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

#define PACKED_COLOR_ALGORITHM_USING_STBIR 1

IJPackedColorImage::IJPackedColorImage() 
	: m_yImage(nullptr)
	, m_uImage(nullptr)
	, m_vImage(nullptr)
	, m_packRate(2)
	, m_fileName()
{}

IJPackedColorImage::IJPackedColorImage(const std::string& fileName)
	: m_yImage(nullptr)
	, m_uImage(nullptr)
	, m_vImage(nullptr)
	, m_packRate(2)
	, m_fileName(fileName)
{}

IJPackedColorImage::IJPackedColorImage(const std::string& fileName, uint8_t packRate)
	: m_yImage(nullptr)
	, m_uImage(nullptr)
	, m_vImage(nullptr)
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

IJResult IJPackedColorImage::Load(std::istream& istream)
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

IJResult IJPackedColorImage::Save(std::ostream& ostream)
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

	SaveHeader(m_yImage);
	m_yImage->DebugSave("output/dbg/y_image.dbg.tga");

	result = m_uImage->Save(ostream);
	assert(result == IJResult::Success);

	SaveHeader(m_uImage);
	m_uImage->DebugSave("output/dbg/u_image.dbg.tga", IJSingleCompImage::G);

	result = m_vImage->Save(ostream);
	assert(result == IJResult::Success);

	SaveHeader(m_vImage);
	m_vImage->DebugSave("output/dbg/v_image.dbg.tga", IJSingleCompImage::B);

	return result;
}

IJResult IJPackedColorImage::PackImage(IJYCbCrImage888* image, uint8_t rate)
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

	size_t size = image->GetPixelData().size();

	std::vector<uint8_t> yComp, uComp, vComp;
	yComp.resize(size);
	uComp.resize(size);
	vComp.resize(size);

	for (size_t i = 0; i < size; i++)
	{
		auto& pixel = (*image->GetPixelData()[i]);
		yComp[i] = pixel[0];
		uComp[i] = pixel[1];
		vComp[i] = pixel[2];
	}

	{
		image->Save("output/dbg/debug_yuv_image.tga");

		IJSingleCompImage dbgimage;
		SaveHeader(&dbgimage);

		std::strstream dbg_ystream((char*)&yComp.front(), yComp.size(), std::ios::in | std::ios::binary);
		dbgimage.ClearPixels();
		dbgimage.Load(dbg_ystream);
		dbgimage.Save("output/dbg/y_full_image.tga"/*, IJSingleCompImage::B*/);

		std::strstream dbg_ustream((char*)&uComp.front(), uComp.size(), std::ios::in | std::ios::binary);
		dbgimage.ClearPixels();
		dbgimage.Load(dbg_ustream);
		dbgimage.DebugSave("output/dbg/u_full_image.tga");

		std::strstream dbg_vstream((char*)&vComp.front(), vComp.size(), std::ios::in | std::ios::binary);
		dbgimage.ClearPixels();
		dbgimage.Load(dbg_vstream);
		dbgimage.DebugSave("output/dbg/v_full_image.tga");
	}

	{
		std::vector<uint8_t> tempVec;
		tempVec.resize(uComp.size());
		int outputSizeW = image->GetWidth() / m_packRate;
		int outputSizeH = image->GetHeight() / m_packRate;
		size_t outputSize = static_cast<size_t>(outputSizeW * outputSizeH);

		stbir__resize_arbitrary(nullptr,
								(const void*)&uComp.front(), image->GetWidth(), image->GetHeight(), 0,
								(void*)&tempVec.front(), outputSizeW, outputSizeH, 0,
								0, 0, 1, 1, nullptr, 1, -1, 0,
								STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
								STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);
		
		std::swap(uComp, tempVec);
		assert(uComp.size());

		stbir__resize_arbitrary(nullptr,
								(const void*)&vComp.front(), image->GetWidth(), image->GetHeight(), 0,
								(void*)&tempVec.front(), outputSizeW, outputSizeH, 0,
								0, 0, 1, 1, nullptr, 1, -1, 0,
								STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
								STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);

		std::swap(vComp, tempVec);
		assert(vComp.size());

		m_uvSize = outputSize;
		uComp.resize(outputSize);
		vComp.resize(outputSize);
	}

	IJResult result = IJResult::UnknownResult;
	m_ySize = yComp.size();

	std::strstream yStream((char*)&yComp.front(), yComp.size(), std::ios::in | std::ios::binary);
	result = m_yImage->Load(yStream);
	assert(result == IJResult::Success);

	std::strstream uvStream((char*)&uComp.front(), uComp.size(), std::ios::in | std::ios::binary);
	result = m_uImage->Load(uvStream);
	assert(result == IJResult::Success);

	std::strstream vStream((char*)&vComp.front(), vComp.size(), std::ios::in | std::ios::binary);
	result = m_vImage->Load(vStream);
	assert(result == IJResult::Success);

	return result;
}

IJResult IJPackedColorImage::UnpackImage(IJYCbCrImage888* image, uint8_t rate)
{
	ASSERT_PTR(image);
	ASSERT_PTR(m_yImage);
	ASSERT_PTR(m_uImage);
	ASSERT_PTR(m_vImage);
	assert(m_yImage->GetSize());
	assert(m_uImage->GetSize());
	assert(m_vImage->GetSize());

	std::vector<uint8_t> rawImage;
	size_t W = static_cast<size_t>(m_header->width);
	size_t H = static_cast<size_t>(m_header->height);
	size_t packedW = static_cast<size_t>(m_header->width / m_packRate);

	for (size_t i = 0, uvi = 0, uvidx = 0; i < W; i++)
	{
		if (i % m_packRate == 0
			&& i != 0)
		{
			++uvi;
		}

		for (size_t j = 0, uvj = 0; j < H; j++)
		{
			auto& yPixel = *m_yImage->GetPixelData()[i*j];
			
			if (j % m_packRate == 0
				&& j != 0)
			{
				++uvj;
			}

			uvidx = uvi + packedW * uvj;
			auto& uPixel = *m_uImage->GetPixelData()[uvidx];
			auto& vPixel = *m_vImage->GetPixelData()[uvidx];

			rawImage.push_back(yPixel[0]);
			rawImage.push_back(yPixel[0]);
			rawImage.push_back(yPixel[0]);
		}
	}

	SaveHeader(image);
	return image->Load(rawImage);
}

IJResult IJPackedColorImage::PackRGBImage(IJRGBImage* image, uint8_t rate)
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

#if defined(_DEBUG)
	yuvImage->Save("output/dbg/yuv_debug_output.tga");
#endif

	result = PackImage(yuvImage, m_packRate);

	if (yuvImage)
	{
		delete yuvImage;
		yuvImage = nullptr;
	}
	return result;
}

IJResult IJPackedColorImage::UnpackRGBImage(IJRGBImage* image, uint8_t rate)
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

void IJPackedColorImage::CreateCompImages()
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

IJResult IJPackedColorImage::Load()
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

IJResult IJPackedColorImage::LoadHeader(IJImageInterface<uint8_t, 3>* image)
{
	ASSERT_PTR(image);

	if (!m_header)
	{
		m_header = new TGAHeader();
		ASSERT_PTR(m_header);
	}

	m_header->idlength			= image->m_header.idlength;
	m_header->colourmaptype		= image->m_header.colourmaptype;
	m_header->datatypecode		= image->GetCompressionType();
	m_header->colourmaporigin	= image->m_header.colourmaporigin;
	m_header->colourmaplength	= image->m_header.colourmaplength;
	m_header->colourmapdepth	= image->m_header.colourmapdepth;
	m_header->x_origin			= image->m_header.x_origin;
	m_header->y_origin			= image->m_header.y_origin;
	m_header->width				= image->GetWidth();
	m_header->height			= image->GetHeight();
	m_header->bitsperpixel		= image->GetBitsPerPixel();
	m_header->imagedescriptor	= image->m_header.imagedescriptor;
	return IJResult::Success;
}

IJResult IJPackedColorImage::SaveHeader(IJImageInterface<uint8_t, 3>* image)
{
	ASSERT_PTR(image);
	ASSERT_PTR(m_header);

	image->m_header.idlength		= m_header->idlength;
	image->m_header.colourmaptype	= m_header->colourmaptype;
	image->m_header.datatypecode	= m_header->datatypecode;
	image->m_header.colourmaporigin	= m_header->colourmaporigin;
	image->m_header.colourmaplength	= m_header->colourmaplength;
	image->m_header.colourmapdepth	= m_header->colourmapdepth;
	image->m_header.x_origin		= m_header->x_origin;
	image->m_header.y_origin		= m_header->y_origin;
	image->m_header.width			= m_header->width;
	image->m_header.height			= m_header->height;
	image->m_header.bitsperpixel	= m_header->bitsperpixel;
	image->m_header.imagedescriptor	= m_header->imagedescriptor;

	return IJResult::Success;
}

IJResult IJPackedColorImage::SaveHeader(IJImageInterface<uint8_t, 1>* image)
{
	ASSERT_PTR(image);
	ASSERT_PTR(m_header);

	image->m_header.idlength		= m_header->idlength;
	image->m_header.colourmaptype	= m_header->colourmaptype;
	image->m_header.datatypecode	= m_header->datatypecode;
	image->m_header.colourmaporigin	= m_header->colourmaporigin;
	image->m_header.colourmaplength	= m_header->colourmaplength;
	image->m_header.colourmapdepth	= m_header->colourmapdepth;
	image->m_header.x_origin		= m_header->x_origin;
	image->m_header.y_origin		= m_header->y_origin;
	image->m_header.width			= m_header->width;
	image->m_header.height			= m_header->height;
	image->m_header.bitsperpixel	= m_header->bitsperpixel;
	image->m_header.imagedescriptor	= m_header->imagedescriptor;

	return IJResult::Success;
}
