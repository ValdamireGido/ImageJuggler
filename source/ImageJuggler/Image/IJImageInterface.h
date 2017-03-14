#pragma once 

#include "IJTypes.h"

#include <string>
#include <vector>
#include <stdint.h>
#include <fstream>
#include <array>

///////////////////////////////////////////////////////////////////////

std::string GetFileExtension(const std::string& fileName);

///////////////////////////////////////////////////////////////////////

template<typename _PixelCompTy, size_t _compsCount>
class IJPixelInterface
{
public:
	static const size_t k_compCount = _compsCount;
	using Comp_t = _PixelCompTy;

public:
	IJPixelInterface() = default;
	IJPixelInterface(const IJPixelInterface&  other) = default;
	IJPixelInterface& operator=(const IJPixelInterface& other) = default;

	IJPixelInterface(IJPixelInterface&& other) = delete;
	IJPixelInterface&	operator=(IJPixelInterface&& other) = delete;

	virtual ~IJPixelInterface() {}

	virtual	std::vector<Comp_t> serialize() const = 0;
	virtual void				deserialize(const std::array<Comp_t, k_compCount>& rawPixel) = 0;

	virtual Comp_t&	operator[](size_t compIdx) = 0;

	inline size_t compCount() const { return k_compCount; }
};

///////////////////////////////////////////////////////////////////////

template<typename _PixelCompTy, size_t _compsCount>
class IJImageInterface
{
	friend struct IJImageTranslator;

public:
	using Pixel_t		= IJPixelInterface<_PixelCompTy, _compsCount>;
	using PixelData_t	= std::vector<Pixel_t*>;

	struct TGAHeader
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

		TGAHeader();
		void deserialize(std::istream& iStream);
		void serialize(std::ostream& oStream);
		operator std::vector<uint8_t>();
	};

public:
	// C'tor
	IJImageInterface() = default;
	IJImageInterface(const IJImageInterface&  other) = default;
	IJImageInterface& operator=(const IJImageInterface&  other) = default;

	IJImageInterface(IJImageInterface&& other) = default;
	IJImageInterface& operator=(IJImageInterface&& other) = default;

	IJImageInterface(IJImageType type);
	IJImageInterface(const std::string& fileName, IJImageType type);

	// D'tor
	virtual ~IJImageInterface();

	// IJImageInterface interface
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
	TGAHeader		m_header;
	PixelData_t		m_pixels;
	size_t			m_imageSize;
	IJImageType		m_imageType;
};

// Inline realization

template <typename _PixelCompTy, size_t _compsCount>
inline IJImageInterface<_PixelCompTy, _compsCount>::TGAHeader::TGAHeader()
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

template <typename _PixelCompTy, size_t _compsCount>
inline void IJImageInterface<_PixelCompTy, _compsCount>::TGAHeader::deserialize(std::istream& iStream)
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

template <typename _PixelCompTy, size_t _compsCount>
inline void IJImageInterface<_PixelCompTy, _compsCount>::TGAHeader::serialize(std::ostream& oStream)
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

template <typename _PixelCompTy, size_t _compsCount>
IJImageInterface<_PixelCompTy, _compsCount>::TGAHeader::operator std::vector<uint8_t>()
{
	return std::vector<uint8_t> 
	{
		(uint8_t)idlength;
		(uint8_t)colourmaptype;
		(uint8_t)datatypecode;
		(uint8_t)(colourmaporigin & 0x00FF);		// 2 bytes
		(uint8_t)(colourmaporigin & 0xFF00) >> 8;	// 
		(uint8_t)(colourmaplength & 0x00FF);		// 2 bytes
		(uint8_t)(colourmaplength & 0xFF00) >> 8;	// 
		(uint8_t)colourmapdepth;
		(uint8_t)(x_origin & 0x00FF);				// 2 bytes
		(uint8_t)(x_origin & 0xFF00) >> 8;			// 
		(uint8_t)(y_origin & 0x00FF);				// 2 bytes
		(uint8_t)(y_origin & 0xFF00) >> 8;			// 
		(uint8_t)(width & 0x00FF);					// 2 bytes
		(uint8_t)(width & 0xFF00) >> 8;				// 
		(uint8_t)(height & 0x00FF);					// 2 bytes
		(uint8_t)(height & 0xFF00) >> 8;			// 
		(uint8_t)bitsperpixel; 
		(uint8_t)imagedescriptor;
	};
}

template <typename _PixelCompTy, size_t _compsCount>
IJImageInterface<_PixelCompTy, _compsCount>::IJImageInterface(IJImageType type)
	: m_fileName()
	, m_header()
	, m_pixels()
	, m_imageSize(0ul)
	, m_imageType(type)
{}

template <typename _PixelCompTy, size_t _compsCount>
IJImageInterface<_PixelCompTy, _compsCount>::IJImageInterface(const std::string& fileName, IJImageType type)
	: m_fileName(fileName)
	, m_header()
	, m_pixels()
	, m_imageSize(0u)
	, m_imageType(type)
{}

template <typename _PixelCompTy, size_t _compsCount>
IJImageInterface<_PixelCompTy, _compsCount>::~IJImageInterface()
{
	for (size_t i = 0, size = m_pixels.size(); i < size; ++i)
	{
		if (m_pixels[i])
		{
			delete m_pixels[i];
			m_pixels[i] = nullptr;
		}
	}

	m_pixels.clear();
}

template <typename _PixelCompTy, size_t _compsCount>
IJResult IJImageInterface<_PixelCompTy, _compsCount>::Load()
{
	if (m_fileName.empty())
	{
		return IJResult::InvalidFileName;
	}

	return Load(m_fileName);
}

template <typename _PixelCompTy, size_t _compsCount>
IJResult IJImageInterface<_PixelCompTy, _compsCount>::Load(const std::string& fileName)
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

template <typename _PixelCompTy, size_t _compsCount>
IJResult IJImageInterface<_PixelCompTy, _compsCount>::Save()
{
	if (m_fileName.empty())
	{
		return IJResult::InvalidFileName;
	}

	return Save(m_fileName);
}

template <typename _PixelCompTy, size_t _compsCount>
IJResult IJImageInterface<_PixelCompTy, _compsCount>::Save(const std::string& fileName)
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

template <typename _PixelCompTy, size_t _compsCount>
inline const std::string& IJImageInterface<_PixelCompTy, _compsCount>::GetFileName() const 
{
	return m_fileName;
}

template <typename _PixelCompTy, size_t _compsCount>
inline IJImageType IJImageInterface<_PixelCompTy, _compsCount>::GetImageType() const 
{
	return m_imageType;
}

template <typename _PixelCompTy, size_t _compsCount>
inline const typename IJImageInterface<_PixelCompTy, _compsCount>::PixelData_t& IJImageInterface<_PixelCompTy, _compsCount>::GetPixelData() const
{
	return m_pixels;
}

template <typename _PixelCompTy, size_t _compsCount>
inline size_t IJImageInterface<_PixelCompTy, _compsCount>::GetSize() const
{
	return m_imageSize;
}

template <typename _PixelCompTy, size_t _compsCount>
inline uint16_t IJImageInterface<_PixelCompTy, _compsCount>::GetWidth() const 
{
	return m_header.width;
}

template <typename _PixelCompTy, size_t _compsCount>
inline uint16_t IJImageInterface<_PixelCompTy, _compsCount>::GetHeight() const
{
	return m_header.height;
}

template <typename _PixelCompTy, size_t _compsCount>
inline CompressionType IJImageInterface<_PixelCompTy, _compsCount>::GetCompressionType() const 
{
	return m_header.datatypecode;
}

template <typename _PixelCompTy, size_t _compsCount>
inline uint8_t IJImageInterface<_PixelCompTy, _compsCount>::GetBitsPerPixel() const
{
	return m_header.bitsperpixel;
}

template <typename _PixelCompTy, size_t _compsCount>
inline IJResult IJImageInterface<_PixelCompTy, _compsCount>::_LoadHeader(std::istream& iStream)
{
	m_header.deserialize(iStream);
	return IJResult::Success;
}

template <typename _PixelCompTy, size_t _compsCount>
inline IJResult IJImageInterface<_PixelCompTy, _compsCount>::_SaveHeader(std::ostream& oStream)
{
	m_header.serialize(oStream);
	return IJResult::Success;
}

template <typename _PixelCompTy, size_t _compsCount>
inline void IJImageInterface<_PixelCompTy, _compsCount>::SetFileName(const std::string& fileName)
{
	m_fileName = fileName;
}

template <typename _PixelCompTy, size_t _compsCount>
inline void IJImageInterface<_PixelCompTy, _compsCount>::SetImageType(IJImageType imageType)
{
	m_imageType = imageType;
}

template <typename _PixelCompTy, size_t _compsCount>
inline void IJImageInterface<_PixelCompTy, _compsCount>::SetSize(size_t size)
{
	m_imageSize = size;
}

template <typename _PixelCompTy, size_t _compsCount>
inline void IJImageInterface<_PixelCompTy, _compsCount>::SetWidth(uint16_t width)
{
	m_header.width = width;
}

template <typename _PixelCompTy, size_t _compsCount>
inline void IJImageInterface<_PixelCompTy, _compsCount>::SetHeight(uint16_t height)
{
	m_header.height = height;
}

template <typename _PixelCompTy, size_t _compsCount>
inline void IJImageInterface<_PixelCompTy, _compsCount>::SetCompressionType(CompressionType type)
{
	m_header.datatypecode = type;
}

template <typename _PixelCompTy, size_t _compsCount>
inline void IJImageInterface<_PixelCompTy, _compsCount>::SetBitsPerPixel(uint8_t bits)
{
	m_header.bitsperpixel = bits;
}

template <typename _PixelCompTy, size_t _compsCount>
inline typename IJImageInterface<_PixelCompTy, _compsCount>::PixelData_t& IJImageInterface<_PixelCompTy, _compsCount>::GetPixelData() 
{
	return m_pixels;
}

template <typename _PixelCompTy, size_t _compsCount>
inline void IJImageInterface<_PixelCompTy, _compsCount>::AddPixel(typename IJImageInterface<_PixelCompTy, _compsCount>::Pixel_t* pPixel)
{
	m_pixels.push_back(pPixel);
}

template <typename _PixelCompTy, size_t _compsCount>
inline void IJImageInterface<_PixelCompTy, _compsCount>::ClearPixels()
{
	m_pixels.clear();
}
