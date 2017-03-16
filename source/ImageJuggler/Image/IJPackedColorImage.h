#pragma once

#include "IJSingleCompImage.h"

class IJYCbCrImage888;

class IJPackedColorImage
{
public:
	IJPackedColorImage();
	IJPackedColorImage(const std::string& fileName);
	IJPackedColorImage(const std::string& fileName, float packedRate);
	~IJPackedColorImage();

	IJResult Load(std::istream& iStream);
	IJResult Save(std::ostream& oStream);

	IJResult PackImage(IJYCbCrImage888* image, float rate = 2.f);
	
	const std::string& GetFileName() const;
	float GetPackRate() const;

private:
	void SetPackRate(float rate);
	void CreateCompImages();

private:
	IJSingleCompImage* m_yImage;
	IJSingleCompImage* m_uImage;
	IJSingleCompImage* m_vImage;
	float m_packRate;

	std::string	m_fileName;
	
};


inline const std::string& IJPackedColorImage::GetFileName() const 
{
	return m_fileName;
}

inline float IJPackedColorImage::GetPackRate() const
{
	return m_packRate;
}

inline void IJPackedColorImage::SetPackRate(float packRate)
{
	m_packRate = packRate;
}
