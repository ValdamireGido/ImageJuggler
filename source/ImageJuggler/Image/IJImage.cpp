#include "IJImage.h"

std::string GetFileExtension(const std::string& fileName) 
{
	std::string ext;
	size_t pos = fileName.find('.');
	if (pos != std::string::npos)
	{
		ext = fileName.substr(pos);
	}

	return ext;
}

IJImage::IJImage(const IJImage&& other)
{
	_copy(other);
}

IJImage::IJImage(IJImage& other)
{
	_copy(other);
}

IJImage& IJImage::operator=(const IJImage&& other) 
{
	_copy(other);
	return *this;
}

IJImage& IJImage::operator=(IJImage& other)
{
	_copy(other);
	return *this;
}

IJImage::IJImage(const std::string& fileName, IJImageType type)
	: m_fileName(fileName)
	, m_imageType(type)
{}

IJImage::~IJImage()
{}


IJResult IJImage::Load(const std::string& fileName)
{
	std::string ext = GetFileExtension(fileName);

	return IJResult::Success;
}

void IJImage::_copy(const IJImage& other)
{
	m_fileName		= other.m_fileName;
	m_imageType		= other.m_imageType;
}