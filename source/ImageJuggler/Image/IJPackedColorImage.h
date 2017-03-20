#pragma once

#include "IJSingleCompImage.h"

struct IJImageInterface<uint8_t, 3>::TGAHeader;
class IJImage;
class IJYCbCrImage888;
class IJRGBImage;

class IJPackedColorImage
{
	using TGAHeader = IJImageInterface<uint8_t, 3>::TGAHeader;
public:
	IJPackedColorImage();
	IJPackedColorImage(const std::string& fileName);
	IJPackedColorImage(const std::string& fileName, uint8_t packedRate);
	~IJPackedColorImage();

	IJResult Load(std::istream& iStream);
	IJResult Save(std::ostream& oStream);

	IJResult PackImage(IJYCbCrImage888* image, uint8_t rate = 2);
	IJResult UnpackImage(IJYCbCrImage888* image, uint8_t rate = 2);

	IJResult PackRGBImage(IJRGBImage* image, uint8_t rate = 2);
	IJResult UnpackRGBImage(IJRGBImage* image, uint8_t rate = 2);

	const std::string& GetFileName() const;
	uint8_t GetPackRate() const;

private:
	void SetPackRate(uint8_t rate);
	void CreateCompImages();

	IJResult Load();
	IJResult Save();

	IJResult LoadHeader(IJImageInterface<uint8_t, 3>* image);
	IJResult SaveHeader(IJImageInterface<uint8_t, 3>* image);
	IJResult SaveHeader(IJImageInterface<uint8_t, 1>* image);

private:
	// TODO: change the storage type for the comps image fields of this class
	//		there is a point to think about - how to store the image data. 
	//		it may be just a vectors, but in case of separate classes we just 
	//		have a special funstionality as for image. but on the other hand
	//		this taks a bit more memmory and may take more time to precess.
	IJSingleCompImage*	m_yImage;
	IJSingleCompImage*	m_uImage;
	IJSingleCompImage*	m_vImage;
	TGAHeader*			m_header;

	size_t				m_ySize;
	size_t				m_uvSize;

	uint8_t				m_packRate;

	std::string	m_fileName;
};


inline const std::string& IJPackedColorImage::GetFileName() const 
{
	return m_fileName;
}

inline uint8_t IJPackedColorImage::GetPackRate() const
{
	return m_packRate;
}

inline void IJPackedColorImage::SetPackRate(uint8_t packRate)
{
	m_packRate = packRate;
}
