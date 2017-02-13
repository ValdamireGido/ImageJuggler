#pragma once 

#include "IJTypes.h"

#include <string>
#include <vector>
#include <stdint.h>
#include <fstream>

///////////////////////////////////////////////////////////////////////

std::string GetFileExtension(const std::string& fileName);

///////////////////////////////////////////////////////////////////////

template<typename _PixelCompTy>
struct Pixel
{
	using Comp_t = _PixelCompTy;

	Pixel() = default;
	Pixel(const Pixel&  other) = default;
	Pixel& operator=(const Pixel& other) = default;

	Pixel(Pixel&& other) = delete;
	Pixel&	operator=(Pixel&& other) = delete;

	virtual ~Pixel() {}

	virtual				operator std::vector<Comp_t>()	const	= 0;
	virtual Comp_t&		operator[](uint32_t compIdx)			= 0;
	virtual uint32_t	compCount()						const	= 0;
};

///////////////////////////////////////////////////////////////////////

template<typename _PixelCompTy>
class IJImage
{
	friend struct IJImageTranslator;

public:
	using Pixel_t		= Pixel<_PixelCompTy>;
	using PixelData_t	= std::vector<Pixel<_PixelCompTy>*>;

	struct Header
	{
		static const uint8_t k_headerSize = 18;

		uint8_t			idlength;
		uint8_t			colourmaptype;
		CompressionType datatypecode;
		uint16_t		colourmaporigin;
		uint16_t		colourmaplength;
		uint8_t			colourmapdepth;
		uint16_t		x_origin;
		uint16_t		y_origin;
		uint16_t		width;
		uint16_t		height;
		uint8_t			bitsperpixel;
		uint8_t			imagedescriptor;

		Header();
		void deserialize(std::istream& iStream);
		void serialize(std::ostream& oStream);
		operator std::vector<uint8_t>();
	};

public:
	// C'tor
	IJImage() = default;
	IJImage(const IJImage&  other) = default;
	IJImage& operator=(const IJImage&  other) = default;

	IJImage(IJImage&& other) = default;
	IJImage& operator=(IJImage&& other) = default;

	IJImage(IJImageType type);
	IJImage(const std::string& fileName, IJImageType type);

	// D'tor
	virtual ~IJImage();

	// IJImage interface
			IJResult	Load();
			IJResult	Load(const std::string& fileName);
	virtual IJResult	Load(	   std::istream& iStream) = 0;
			IJResult	Save();
			IJResult	Save(const std::string& fileName);
	virtual IJResult	Save(	   std::ostream& oStream) = 0;

	const std::string&	GetFileName()		const;
		  IJImageType	GetImageType()		const;
	const PixelData_t&	GetPixelData()		const;
		  size_t		GetSize()			const;
		  uint16_t		GetWidth()			const;
		  uint16_t		GetHeight()			const;
		  CompressionType GetCompressionType() const;
		  uint8_t		GetBitsPerPixel()	const;

protected:
	IJResult			_LoadHeader(std::istream& iStream);
	IJResult			_SaveHeader(std::ostream& oStream);

	void				SetFileName(const std::string& fileName);
	void				SetImageType(IJImageType type);
	void				SetSize(size_t size);
	void				SetWidth(uint16_t width);
	void				SetHeight(uint16_t height);
	void				SetCompressionType(CompressionType type);
	void				SetBitsPerPixel(uint8_t bits);

	PixelData_t&		GetPixelData();
	void				AddPixel(Pixel_t* pixel);
	void				ClearPixels();

private:
	std::string		m_fileName;
	Header			m_header;
	PixelData_t		m_pixels;
	size_t			m_imageSize;
	IJImageType		m_imageType;
};

// Inline realization

template <typename _PixelCompTy>
inline IJImage<_PixelCompTy>::Header::Header()
	: idlength(uint8_t(0))
	, colourmaptype(uint8_t(0))
	, datatypecode(CompressionType::UncompressedRGB)
	, colourmaporigin(uint16_t(0))
	, colourmaplength(uint16_t(0))
	, colourmapdepth( uint8_t(0))
	, x_origin(uint16_t(0))
	, y_origin(uint16_t(0))
	, width(uint16_t(0))
	, height(uint16_t(0))
	, bitsperpixel(uint8_t(24))
	, imagedescriptor(uint8_t(0))
{}

template <typename _PixelCompTy>
inline void IJImage<_PixelCompTy>::Header::deserialize(std::istream& iStream)
{
	iStream.read((char*)&idlength		, 1);
	iStream.read((char*)&colourmaptype	, 1);
	iStream.read((char*)&datatypecode	, 1);
	iStream.read((char*)&colourmaporigin, 2);
	iStream.read((char*)&colourmaplength, 2);
	iStream.read((char*)&colourmapdepth	, 1);
	iStream.read((char*)&x_origin		, 2);
	iStream.read((char*)&y_origin		, 2);
	iStream.read((char*)&width			, 2);
	iStream.read((char*)&height			, 2);
	iStream.read((char*)&bitsperpixel	, 1);
	iStream.read((char*)&imagedescriptor, 1);
}

template <typename _PixelCompTy>
inline void IJImage<_PixelCompTy>::Header::serialize(std::ostream& oStream)
{
	oStream.write((const char*)&idlength		, 1);
	oStream.write((const char*)&colourmaptype	, 1);
	oStream.write((const char*)&datatypecode	, 1);
	oStream.write((const char*)&colourmaporigin	, 2);
	oStream.write((const char*)&colourmaplength	, 2);
	oStream.write((const char*)&colourmapdepth	, 1);
	oStream.write((const char*)&x_origin		, 2);
	oStream.write((const char*)&y_origin		, 2);
	oStream.write((const char*)&width			, 2);
	oStream.write((const char*)&height			, 2);
	oStream.write((const char*)&bitsperpixel	, 1);
	oStream.write((const char*)&imagedescriptor	, 1);
}

template <typename _PixelCompTy>
IJImage<_PixelCompTy>::Header::operator std::vector<uint8_t>()
{
	return std::vector<uint8_t> 
	{
		(uint8_t)idlength;
		(uint8_t)colourmaptype;
		(uint8_t)datatypecode;
		(uint8_t)(colourmaporigin & 0x00FF);		// 2 bytes
		(uint8_t)(colourmaporigin & 0xFF00) / 256;	// 2 bytes
		(uint8_t)(colourmaplength & 0x00FF);		// 2 bytes
		(uint8_t)(colourmaplength & 0xFF00) / 256;	// 2 bytes
		(uint8_t)colourmapdepth;
		(uint8_t)(x_origin & 0x00FF);				// 2 bytes
		(uint8_t)(x_origin & 0xFF00) / 256;		// 2 bytes
		(uint8_t)(y_origin & 0x00FF);				// 2 bytes
		(uint8_t)(y_origin & 0xFF00) / 256;		// 2 bytes
		(uint8_t)(width & 0x00FF);					// 2 bytes
		(uint8_t)(width & 0xFF00) / 256;			// 2 bytes
		(uint8_t)(height & 0x00FF);				// 2 bytes
		(uint8_t)(height & 0xFF00) / 256;			// 2 bytes
		(uint8_t)bitsperpixel; 
		(uint8_t)imagedescriptor;
	};
}

template <typename _PixelCompTy>
IJImage<_PixelCompTy>::IJImage(IJImageType type)
	: m_fileName()
	, m_header()
	, m_pixels()
	, m_imageSize(0ul)
	, m_imageType(type)
{}

template <typename _PixelCompTy>
IJImage<_PixelCompTy>::IJImage(const std::string& fileName, IJImageType type)
	: m_fileName(fileName)
	, m_header()
	, m_pixels()
	, m_imageSize(0u)
	, m_imageType(type)
{}

template <typename _PixelCompTy>
IJImage<_PixelCompTy>::~IJImage()
{}

template <typename _PixelCompTy>
IJResult IJImage<_PixelCompTy>::Load()
{
	if (m_fileName.empty())
	{
		return IJResult::InvalidFileName;
	}

	return Load(m_fileName);
}

template <typename _PixelCompTy>
IJResult IJImage<_PixelCompTy>::Load(const std::string& fileName)
{
	if (fileName.empty())
	{
		return IJResult::InvalidFileName;
	}

	SetFileName(fileName);
	std::ifstream inputFileStream;
	inputFileStream.open(fileName, std::ios::in | std::ios::binary);
	if (!inputFileStream.is_open())
	{
		return IJResult::UnableToOpenFile;
	}

	IJResult result = _LoadHeader(inputFileStream);
	result = Load(inputFileStream);
	inputFileStream.close();
	return result;
}

template <typename _PixelCompTy>
IJResult IJImage<_PixelCompTy>::Save()
{
	if (m_fileName.empty())
	{
		return IJResult::InvalidFileName;
	}

	return Save(m_fileName);
}

template <typename _PixelCompTy>
IJResult IJImage<_PixelCompTy>::Save(const std::string& fileName)
{
	if (fileName.empty())
	{
		return IJResult::InvalidFileName;
	}

	SetFileName(fileName);
	std::ofstream outputFile(fileName, std::ios::out | std::ios::binary);
	IJResult result = _SaveHeader(outputFile);
	result = Save(outputFile);
	outputFile.close();
	return result;
}

template <typename _PixelCompTy>
inline const std::string& IJImage<_PixelCompTy>::GetFileName() const 
{
	return m_fileName;
}

template <typename _PixelCompTy>
inline IJImageType IJImage<_PixelCompTy>::GetImageType() const 
{
	return m_imageType;
}

template <typename _PixelCompTy>
inline const typename IJImage<_PixelCompTy>::PixelData_t& IJImage<_PixelCompTy>::GetPixelData() const
{
	return m_pixels;
}

template <typename _PixelCompTy>
inline size_t IJImage<_PixelCompTy>::GetSize() const
{
	return m_imageSize;
}

template <typename _PixelCompTy>
inline uint16_t IJImage<_PixelCompTy>::GetWidth() const 
{
	return m_header.width;
}

template <typename _PixelCompTy>
inline uint16_t IJImage<_PixelCompTy>::GetHeight() const
{
	return m_header.height;
}

template <typename _PixelCompTy>
inline CompressionType IJImage<_PixelCompTy>::GetCompressionType() const 
{
	return m_header.datatypecode;
}

template <typename _PixelCompTy>
inline uint8_t IJImage<_PixelCompTy>::GetBitsPerPixel() const
{
	return m_header.bitsperpixel;
}

template <typename _PixelCompTy>
inline IJResult IJImage<_PixelCompTy>::_LoadHeader(std::istream& iStream)
{
	m_header.deserialize(iStream);
	return IJResult::Success;
}

template <typename _PixelCompTy>
inline IJResult IJImage<_PixelCompTy>::_SaveHeader(std::ostream& oStream)
{
	m_header.serialize(oStream);
	return IJResult::Success;
}

template <typename _PixelCompTy>
inline void IJImage<_PixelCompTy>::SetFileName(const std::string& fileName)
{
	m_fileName = fileName;
}

template <typename _PixelCompTy>
inline void IJImage<_PixelCompTy>::SetImageType(IJImageType imageType)
{
	m_imageType = imageType;
}

template <typename _PixelCompTy>
inline void IJImage<_PixelCompTy>::SetSize(size_t size)
{
	m_imageSize = size;
}

template <typename _PixelCompTy>
inline void IJImage<_PixelCompTy>::SetWidth(uint16_t width)
{
	m_header.width = width;
}

template <typename _PixelCompTy>
inline void IJImage<_PixelCompTy>::SetHeight(uint16_t height)
{
	m_header.height = height;
}

template <typename _PixelCompTy>
inline void IJImage<_PixelCompTy>::SetCompressionType(CompressionType type)
{
	m_header.datatypecode = type;
}

template <typename _PixelCompTy>
inline void IJImage<_PixelCompTy>::SetBitsPerPixel(uint8_t bits)
{
	m_header.bitsperpixel = bits;
}

template <typename _PixelCompTy>
inline typename IJImage<_PixelCompTy>::PixelData_t& IJImage<_PixelCompTy>::GetPixelData() 
{
	return m_pixels;
}

template <typename _PixelCompTy>
inline void IJImage<_PixelCompTy>::AddPixel(typename IJImage<_PixelCompTy>::Pixel_t* pPixel)
{
	m_pixels.push_back(pPixel);
}

template <typename _PixelCompTy>
inline void IJImage<_PixelCompTy>::ClearPixels()
{
	m_pixels.clear();
}
