#include "LCImage.h"

LCImage::LCImage(const LCImage&& other)
{
	_copy(other);
}

LCImage::LCImage(LCImage& other)
{
	_copy(other);
}

LCImage& LCImage::operator=(const LCImage&& other) 
{
	_copy(other);
	return *this;
}

LCImage& LCImage::operator=(LCImage& other)
{
	_copy(other);
	return *this;
}

LCImage::LCImage(const std::string& fileName, ImageType type)
	: m_fileName(fileName)
	, m_imageType(type)
{}

LCImage::~LCImage()
{}


Result LCImage::Load(const std::string& fileName)
{}

void LCImage::_copy(const LCImage& other)
{
	m_fileName		= other.m_fileName;
	m_imageType		= other.m_imageType;
}