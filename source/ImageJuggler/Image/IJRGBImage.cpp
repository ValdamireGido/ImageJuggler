#include "IJRGBImage.h"

#include <fstream>
#include <vector>
#include <strstream>

// IJRGBImage

IJRGBImage::IJRGBImage()
	: IJImage(IJImageType::RGB)
{}

IJRGBImage::IJRGBImage(const std::string& fileName)
	: IJImage(fileName, IJImageType::RGB)
{}

IJRGBImage::IJRGBImage(const std::vector<PixelComp_t>& rawImage)
{
	SetSize(rawImage.size());
	IJResult result = Load(rawImage);
	assert(result == IJResult::Success);
}
