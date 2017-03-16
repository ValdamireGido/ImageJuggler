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

	IJResult PackRGBImage(IJRGBImage* image, float rate = 2.f);
	IJResult UnpackRGBImage(IJRGBImage* image, float rate = 2.f);

	const std::string& GetFileName() const;
	float GetPackRate() const;

private:
	void SetPackRate(float rate);
	void CreateCompImages();

	IJResult Load();
	IJResult Save();

private:
	// TODO: change the storage type for the comps image fields of this class
	//		there is a point to think about - how to store the image data. 
	//		it may be just a vectors, but in case of separate classes we just 
	//		have a special funstionality as for image. but on the other hand
	//		this taks a bit more memmory and may take more time to precess.
	IJSingleCompImage* m_yImage;
	IJSingleCompImage* m_uvImage;
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
