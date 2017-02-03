#pragma once

#include "IJTypes.h"
#include "Image/IJRGBImage.h"
#include "Image/IJYCbCrImage.h"

class IJImageTranslator
{
	static IJResult RGBToYCbCr(IJRGBImage* input, IJYCbCrImage* output);
};