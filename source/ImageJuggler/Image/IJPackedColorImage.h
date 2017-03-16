#pragma once

#include "IJSingleCompImage.h"

class IJYCbCrImage888;
class IJRGBImage;

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
	IJResult UnpackImage(IJYCbCrImage888* image, float rate = 2.f);

	IJResult PackRGBImabe(IJRGBImage* image, float rate = 2.f);
	IJResult UnpackRGBImage(IJRGBImage* image, float rate = 2.f);

	const std::string& GetFileName() const;
	float GetPackRate() const;

private:
	void SetPackRate(float rate);
	void CreateCompImages();

	IJResult Load();
	IJResult Save();

private:
	IJSingleCompImage* m_yImage;
	// TODO: change the storage type for the comps image fields of this class
	IJSingleCompImage* m_uvImage; // THIS IS THE WRONG USE RIGHT NOW!!! NEED TO REWORK
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
