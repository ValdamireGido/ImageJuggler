#pragma once 

#include "LumiComp/Types.h"

#include <string>
#include <vector>
#include <stdint.h>

class LCImage
{
public:
	enum class ImageType : uint8_t
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
	};
	using PixelData	= std::vector<Pixel*>;

public:
	// C'tor
	LCImage() = default;
	LCImage(const LCImage&& other);
	LCImage(LCImage& other);
	LCImage& operator=(const LCImage&& other);
	LCImage& operator=(LCImage& other);

	LCImage(const std::string& fileName, ImageType type);

	// D'tor
	virtual ~LCImage();

	// LCImage interface
	Result					Load(const std::string& fileName);

	const std::string&		GetFileName() const;
	ImageType				GetImageType() const;

protected:
	void					SetFileName(const std::string& fileName);
	void					SetImageType(ImageType type);

private:
	void _copy(const LCImage& other);

private:
	ImageType		m_imageType;
	std::string		m_fileName;
	PixelData		m_pixels;
};

inline const std::string& LCImage::GetFileName() const 
{
	return m_fileName;
}

inline void LCImage::SetFileName(const std::string& fileName)
{
	m_fileName = fileName;
}

inline LCImage::ImageType LCImage::GetImageType() const
{
	return m_imageType;
}

inline void LCImage::SetImageType(LCImage::ImageType type)
{
	m_imageType = type;
}
