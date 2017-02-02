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
	using Comp_t	= _PixelCompTy;

	Pixel()= default;
	Pixel(const Pixel&  other)	= default;
	Pixel(		Pixel&&	other)	= delete;
	Pixel&	operator=(const Pixel&  other)	= default;
	Pixel&	operator=(	    Pixel&&	other)	= delete;
	virtual ~Pixel() {}

	virtual				operator std::vector<Comp_t>()	const	= 0;
	virtual Comp_t		operator[](uint32_t compIdx)	const	= 0;
	virtual uint32_t	compCount()						const	= 0;
};

///////////////////////////////////////////////////////////////////////

template<typename _PixelCompTy>
class IJImage
{
public:
	using Pixel_t		= Pixel<_PixelCompTy>;
	using PixelData_t	= std::vector<Pixel<_PixelCompTy>*>;

public:
	// C'tor
	IJImage() = default;
	IJImage(const IJImage&  other) = default;
	IJImage(	  IJImage&& other) = default;
	IJImage& operator=(const IJImage&  other) = default;
	IJImage& operator=(		 IJImage&& other) = default;

	IJImage(const std::string& fileName, IJImageType type);

	// D'tor
	virtual ~IJImage();

	// IJImage interface
			IJResult		Load();
			IJResult		Load(const std::string& fileName);
	virtual IJResult		Load(	   std::istream& iStream) = 0;
			IJResult		Save();
			IJResult		Save(const std::string& fileName);
	virtual IJResult		Save(	   std::ostream& oStream) = 0;

	const std::string&		GetFileName()	const;
		  IJImageType		GetImageType()	const;
	const PixelData_t&		GetPixelData()	const;
		  size_t			GetImageSize()	const;

protected:
	void					SetFileName(const std::string& fileName);
	void					SetImageType(IJImageType type);
	void					SetImageSize(size_t size);

	PixelData_t&			GetPixelData();
	void					AddPixel(Pixel_t* pixel);
	void					ClearPixels();

private:
	std::string		m_fileName;
	PixelData_t		m_pixels;
	size_t			m_imageSize;
	IJImageType		m_imageType;
};

// Inline realization

template <typename _PixelCompTy>
IJImage<_PixelCompTy>::IJImage(const std::string& fileName, IJImageType type)
	: m_fileName(fileName)
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
	if (m_fileName.emtpy())
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

	IJResult result = Load(inputFileStream);
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
	std::ifstream outputFile;
	outputFile.open(fileName, std::ios::in | std::ios::binary);
	if (!outputFile.empty())
	{
		return IJResult::UnableToOpenFile;
	}

	IJResult result = Load(outputFile);
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
inline size_t IJImage<_PixelCompTy>::GetImageSize() const
{
	return m_imageSize;
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
inline void IJImage<_PixelCompTy>::SetImageSize(size_t size)
{
	m_imageSize = size;
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
