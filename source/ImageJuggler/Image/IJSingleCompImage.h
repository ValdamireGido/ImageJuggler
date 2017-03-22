#pragma once 

#include "IJImageInterface.h"


/* 
		YUV Single Component Image
		Has three different collections for every component. 
*/
class IJSingleCompImage
	: public IJImageInterface<uint8_t, 1>
{
public:
	using PixelData_t = std::vector<uint8_t>;

	enum CompIdx
	{
		R = 1, 
		G = 1 << 1, 
		B = 1 << 2
	};

public:
	IJSingleCompImage();
	virtual ~IJSingleCompImage() {}

	IJResult Load(std::istream& iStream) override;
	IJResult Load(std::istream& istream, size_t size);
	IJResult Save(std::ostream& oStream) override;

	uint8_t& operator [] (size_t idx);

	IJResult DebugSave(const std::string& fileName, uint8_t compsToDump = R|G|B);

	using IJImageInterface<uint8_t, 1>::GetData;
	using IJImageInterface<uint8_t, 1>::Load;
	using IJImageInterface<uint8_t, 1>::Save;
	using IJImageInterface<uint8_t, 1>::GetSize;
};

inline uint8_t& IJSingleCompImage::operator [] (size_t idx)
{
	return GetData()[idx];
}
