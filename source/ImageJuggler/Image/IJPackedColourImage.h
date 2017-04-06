#pragma once

#include "IJSingleCompImage.h"

class IJImage;
class IJYCbCrImage888;
class IJRGBImage;


/*
		IJPackedColourImage class 
		Stores the image data in packed format: 
		- converted to YUV colour space image 
		- splited to three compenents 
		- U and V components resized to the original image size / pack rate of the final image
		- and stored in the next order: 
		---------------- -------- --------
		|			   | |  U   | |  V   |
		|			   | | comps| |comps |
		|			   | |		| |		 |
		|	Y comps    | -------- --------
		|			   |
		|			   |
		----------------

		(splited by component). 
*/
class IJPackedColourImage
{
	friend struct IJYuvPackedConverter;

	static const int k_defaultPackRate = 8;
	static const int k_workingThreadCount = 8;
	using TGAHeader = IJImageInterface<uint8_t, 3>::TGAHeader;
public:
	IJPackedColourImage();
	IJPackedColourImage(const std::string& fileName);
	IJPackedColourImage(const std::string& fileName, uint8_t packedRate);
	~IJPackedColourImage();

	IJResult Load(std::istream& iStream);
	IJResult Save(std::ostream& oStream);

	IJResult PackImage(IJYCbCrImage888* image, uint8_t rate = k_defaultPackRate);
	IJResult UnpackImage(IJYCbCrImage888* image, uint8_t rate = k_defaultPackRate);

	IJResult PackRGBImage(IJRGBImage* image, uint8_t rate = k_defaultPackRate);
	IJResult UnpackRGBImage(IJRGBImage* image, uint8_t rate = k_defaultPackRate);

	const std::string& GetFileName() const;
	uint8_t GetPackRate() const;

private:
	void SetPackRate(uint8_t rate);
	void CreateCompImages();

	IJResult Load();
	IJResult Save();

	template <typename _PixelTy, size_t _nComps>
	IJResult LoadHeader(IJImageInterface<_PixelTy, _nComps>* image);

	template <typename _PixelTy, size_t _nComps>
	IJResult SaveHeader(IJImageInterface<_PixelTy, _nComps>* image);

private:
	// TODO: change the storage type for the comps image fields of this class
	//		there is a point to think about - how to store the image data. 
	//		it may be just a vectors, but in case of separate classes we just 
	//		have a special funstionality as for image. but on the other hand
	//		this taks a bit more memmory and may take more time to precess.
	IJSingleCompImage*	m_yImage;
	IJSingleCompImage*	m_uImage;
	IJSingleCompImage*	m_vImage;
	TGAHeader*			m_header;

	size_t				m_ySize;
	size_t				m_uvSize;

	uint8_t				m_packRate;

	std::string	m_fileName;
};


inline const std::string& IJPackedColourImage::GetFileName() const 
{
	return m_fileName;
}

inline uint8_t IJPackedColourImage::GetPackRate() const
{
	return m_packRate;
}

inline void IJPackedColourImage::SetPackRate(uint8_t packRate)
{
	m_packRate = packRate;
}

template <typename _PixelTy, size_t _nComps>
IJResult IJPackedColourImage::LoadHeader(IJImageInterface<_PixelTy, _nComps>* image)
{
	if (!image)
	{
		return IJResult::BadMemoryPointer;
	}

	if (!m_header)
	{
		m_header = new TGAHeader();
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

template <typename _PixelTy, size_t _nComps>
IJResult IJPackedColourImage::SaveHeader(IJImageInterface<_PixelTy, _nComps>* image)
{
	if (!image)
	{
		return IJResult::BadMemoryPointer;
	}

	if (!m_header)
	{
		return IJResult::BadMemoryPointer;
	}

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
