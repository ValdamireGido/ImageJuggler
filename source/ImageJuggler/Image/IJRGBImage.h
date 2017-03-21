#pragma once 

#include "IJImage.h"

#include <array>
#include <assert.h>

// IJRGBPixel

class IJRGBPixel : public IJPixel {};

// IJRGBImage

class IJRGBImage
	: public IJImage
{
public:
	IJRGBImage();
	IJRGBImage(const std::string& fileName);
	IJRGBImage(const std::vector<PixelComp_t>& rawImage);
};
