#pragma once 

#include "IJTypes.h"

#include <string>
#include <vector>
#include <stdint.h>

std::string GetFileExtension(const std::string& fileName);

class IJImage
{
public:
	enum class IJImageType : uint8_t
	{
		RGB = 0, 
		YCbCr, 
		YCoCg
	};

	struct Pixel
	{
		Pixel() = default;
		Pixel(Pixel&) = delete;
		Pixel& operator=(Pixel&) = delete;
		virtual ~Pixel() {}

		virtual float operator[](uint32_t compIdx) const = 0;
		virtual uint32_t compCount() const = 0;
	};
	using PixelData	= std::vector<Pixel*>;

public:
	// C'tor
	IJImage() = default;
	IJImage(const IJImage&& other);
	IJImage(IJImage& other);
	IJImage& operator=(const IJImage&& other);
	IJImage& operator=(IJImage& other);

	IJImage(const std::string& fileName, IJImageType type);

	// D'tor
	virtual ~IJImage();

	// IJImage interface
	IJResult				Load(const std::string& fileName);

	const std::string&		GetFileName() const;
	IJImageType				GetImageType() const;
	const PixelData&		GetPixelData() const;
	size_t					GetImageSize() const;

protected:
	void					SetFileName(const std::string& fileName);
	void					SetImageType(IJImageType type);
	void					SetImageSize(size_t size);

	PixelData&				GetPixelData();
	void					AddPixel(Pixel* pixel);
	void					ClearPixels();

private:
	void _copy(const IJImage& other);

private:
	IJImageType		m_imageType;
	std::string		m_fileName;
	PixelData		m_pixels;
	size_t			m_imageSize;
};

inline const std::string& IJImage::GetFileName() const 
{
	return m_fileName;
}

inline void IJImage::SetFileName(const std::string& fileName)
{
	m_fileName = fileName;
}

inline IJImage::IJImageType IJImage::GetImageType() const
{
	return m_imageType;
}

inline void IJImage::SetImageType(IJImage::IJImageType type)
{
	m_imageType = type;
}

inline size_t IJImage::GetImageSize() const
{
	return m_imageSize;
}

inline void IJImage::SetImageSize(size_t size)
{
	m_imageSize = size;
}

inline void IJImage::AddPixel(Pixel* pixel)
{
	m_pixels.push_back(pixel);
}

inline void IJImage::ClearPixels()
{
	m_pixels.clear();
}

inline const IJImage::PixelData& IJImage::GetPixelData() const
{
	return m_pixels;
}

inline IJImage::PixelData& IJImage::GetPixelData() 
{
	return m_pixels;
}
