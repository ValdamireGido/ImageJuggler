#pragma once 

#include "IJTypes.h"

#include <string>
#include <vector>
#include <stdint.h>

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

	virtual Comp_t		operator[](uint32_t compIdx) const	= 0;
	virtual uint32_t	compCount()					 const	= 0;
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
	void					AddPixel(Pixel_t* pixel);
	void					ClearPixels();

private:
	void _copy(const IJImage& other);

private:
	std::string		m_fileName;
	PixelData_t		m_pixels;
	size_t			m_imageSize;
	IJImageType		m_imageType;
};

// Inline realization

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