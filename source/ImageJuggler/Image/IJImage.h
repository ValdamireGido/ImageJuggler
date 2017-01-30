#pragma once 

#include "IJTypes.h"

#include <string>
#include <vector>
#include <stdint.h>

std::string GetFileExtension(const std::string& fileName);

template <typename _PixelCompTy>
class IJImage
{
public:
	template<typename _PixelCompTy>
	struct Pixel
	{
		using Comp_t	= _PixelCompTy;

		Pixel() = default;
		Pixel(Pixel&) = delete;
		Pixel& operator=(Pixel&) = delete;
		virtual ~Pixel() {}

		virtual Comp_t		operator[](uint32_t compIdx) const	= 0;
		virtual uint32_t	compCount ()				 const	= 0;
	};

	using PixelComp_t	= _PixelCompTy;
	using Pixel_t		= Pixel<PixelComp_t>;
	using PixelData_t	= std::vector<Pixel_t*>;

public:
	// C'tor
	IJImage() = default;
	IJImage(const IJImage&& other);
	IJImage(	  IJImage&  other);
	IJImage& operator=(const IJImage&& other);
	IJImage& operator=(		 IJImage&  other);

	IJImage(const std::string& fileName, IJImageType type);

	// D'tor
	virtual ~IJImage();

	// IJImage interface
			IJResult		Load();
	virtual IJResult		Load(const std::string& fileName) = 0;
			IJResult		Save();
	virtual IJResult		Save(const std::string& fileName) = 0;

	const std::string&		GetFileName()	const;
		  IJImageType		GetImageType()	const;
	const PixelData_t&		GetPixelData()	const;
		  size_t			GetImageSize()	const;

protected:
	void					SetFileName(const std::string& fileName);
	void					SetImageType(IJImageType type);
	void					SetImageSize(size_t size);

	PixelData_t&			GetPixelData();
	void					AddPixel(Pixel<_PixelCompTy>* pixel);
	void					ClearPixels();

private:
	void _copy(const IJImage<PixelComp_t>& other);

private:
	std::string		m_fileName;
	PixelData_t		m_pixels;
	size_t			m_imageSize;
	IJImageType		m_imageType;
};

template<typename _PixelCompTy>
IJImage<_PixelCompTy>::IJImage(const IJImage&& other)
{
	_copy(other);
}

template<typename _PixelCompTy>
IJImage<_PixelCompTy>::IJImage(IJImage& other)
{
	_copy(other);
}

template<typename _PixelCompTy>
IJImage<_PixelCompTy>& IJImage<_PixelCompTy>::operator=(const IJImage&& other) 
{
	_copy(other);
	return *this;
}

template<typename _PixelCompTy>
IJImage<_PixelCompTy>& IJImage<_PixelCompTy>::operator=(IJImage& other)
{
	_copy(other);
	return *this;
}

template<typename _PixelCompTy>
IJImage<_PixelCompTy>::IJImage(const std::string& fileName, IJImageType type)
	: m_fileName(fileName)
	, m_imageType(type)
{}

template<typename _PixelCompTy>
IJImage<_PixelCompTy>::~IJImage()
{}

template<typename _PixelCompTy>
IJResult IJImage<_PixelCompTy>::Load() 
{
	if (m_fileName.empty())
	{
		return IJResult::UnableToLoadFile;
	}

	return Load(m_fileName);
}

template<typename _PixelCompTy>
IJResult IJImage<_PixelCompTy>::Save()
{
	if (m_fileName.empty())
	{
		return IJResult::UnableToSaveFile;
	}

	return Save(m_fileName);
}

template<typename _PixelCompTy>
IJResult IJImage<_PixelCompTy>::Load(const std::string& fileName)
{
	std::string ext = GetFileExtension(fileName);

	return IJResult::Success;
}

template<typename _PixelCompTy>
void IJImage<_PixelCompTy>::_copy(const IJImage<_PixelCompTy>& other)
{
	m_fileName		= other.m_fileName;
	m_imageType		= other.m_imageType;
	m_imageSize		= other.m_imageSize;
	std::copy(other.m_pixels.begin(), other.m_pixels.end(), 
			  m_pixels.begin());
}

template <typename _PixelCompTy>
const std::string& IJImage<_PixelCompTy>::GetFileName() const 
{
	return m_fileName;
}

template <typename _PixelCompTy>
void IJImage<_PixelCompTy>::SetFileName(const std::string& fileName)
{
	m_fileName = fileName;
}

template<typename _PixelCompTy>
IJImageType IJImage<_PixelCompTy>::GetImageType() const
{
	return m_imageType;
}

template<typename _PixelCompTy>
void IJImage<_PixelCompTy>::SetImageType(IJImageType type)
{
	m_imageType = type;
}

template<typename _PixelCompTy>
size_t IJImage<_PixelCompTy>::GetImageSize() const
{
	return m_imageSize;
}

template<typename _PixelCompTy>
void IJImage<_PixelCompTy>::SetImageSize(size_t size)
{
	m_imageSize = size;
}

template<typename _PixelCompTy>
void IJImage<_PixelCompTy>::AddPixel(Pixel<_PixelCompTy>* pixel)
{
	m_pixels.push_back(pixel);
}

template<typename _PixelCompTy>
void IJImage<_PixelCompTy>::ClearPixels()
{
	m_pixels.clear();
}

template<typename _PixelCompTy>
const typename IJImage<_PixelCompTy>::PixelData_t& IJImage<_PixelCompTy>::GetPixelData() const
{
	return m_pixels;
}

template<typename _PixelCompTy>
typename IJImage<_PixelCompTy>::PixelData_t& IJImage<_PixelCompTy>::GetPixelData() 
{
	return m_pixels;
}
