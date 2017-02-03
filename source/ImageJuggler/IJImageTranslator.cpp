#include "IJImageTranslator.h"
#include "Image/IJRGBImage.h"
#include "Image/IJYCbCrImage444.h"
#include "Image/IJYCbCrImage422.h"

#define PROFILING_ENABLED 1
#include "../../prj/Test/Profiling.h"

#define ASSERT_PTR(ptr)\
do {\
	assert(ptr);\
	if (!ptr) {\
		return IJResult::BadMemoryPointer;\
	}\
}while(false)

std::array<uint8_t, 3> IJImageTranslator::TranslateRGBPixelToYBR(const std::vector<uint8_t>& rgbPixel)
{
	std::array<uint8_t, 3> ybrPixel;
	ybrPixel[0] = uint8_t(16  + ( 65.481 * rgbPixel[0]  + 128.553 * rgbPixel[1]  + 24.966 * rgbPixel[2]));
	ybrPixel[1] = uint8_t(128 + (-37.797 * rgbPixel[0]  +  74.203 * rgbPixel[1]  +  112.0 * rgbPixel[2]));
	ybrPixel[2] = uint8_t(128 + (  112.0 * rgbPixel[0]  +  18.214 * rgbPixel[1]  + 18.214 * rgbPixel[2]));
	return ybrPixel;
}

void IJImageTranslator::TranslateRGBPixelToYBR(IJRGBPixel* rgbPixel, IJYCbCrPixel444* ybrPixel)
{
	*ybrPixel = IJYCbCrPixel444(TranslateRGBPixelToYBR(static_cast<std::vector<uint8_t> >(*rgbPixel)));
}

std::array<uint8_t, 3> IJImageTranslator::TranslateYBRPixelToRGB(const std::vector<uint8_t>& ybrPixel)
{
	std::array<uint8_t, 3> rgbPixel;
	rgbPixel[0] = uint8_t(298.082 * ybrPixel[0] / 256									+ 408.583 * ybrPixel[2]		- 222.921);
	rgbPixel[1] = uint8_t(298.082 * ybrPixel[0] / 256	- 100.291 * ybrPixel[1] / 256	- 208.12  * ybrPixel[2]		+ 135.576);
	rgbPixel[2] = uint8_t(298.082 * ybrPixel[0] / 256	+ 516.412 * ybrPixel[1] / 256								- 276.836);
	return rgbPixel;
}

void IJImageTranslator::TranslateYBRPixelToRGB(IJYCbCrPixel444* ybrPixel, IJRGBPixel* rgbPixel)
{
	*rgbPixel = IJRGBPixel(TranslateYBRPixelToRGB(static_cast<std::vector<uint8_t> >(*ybrPixel)));
}

IJResult IJImageTranslator::RGBToYCbCr444(IJRGBImage* input, IJYCbCrImage444* output)
{
	ASSERT_PTR(input);
	ASSERT_PTR(output);

	assert(input->GetImageSize() != 0);
	if (input->GetImageSize() == 0)
	{
		return IJResult::ImageIsEmtpy;
	}

	for (size_t idx = 0; idx < input->GetImageSize() / IJRGBPixel::k_compCount; idx++)
	{
		IJRGBPixel* rgbPixel = static_cast<IJRGBPixel*>(input->GetPixelData()[idx]);
		assert(rgbPixel);
		if (rgbPixel == nullptr)
		{
			return IJResult::InvalidImageFormat;
		}

		// Creating new ybr pixel
		IJYCbCrPixel444* ybrPixel = new IJYCbCrPixel444();
		assert(ybrPixel);
		if (ybrPixel != nullptr)
		{
			TranslateRGBPixelToYBR(rgbPixel, ybrPixel);
			output->AddPixel(ybrPixel);
			output->SetImageSize(output->GetImageSize() + IJYCbCrPixel444::k_compCount);
		}
	}

	assert(output->GetImageSize() != 0);
	if (output->GetImageSize() == 0)
	{
		return IJResult::UnableToTranslateImage;
	}

	return IJResult::Success;
}

IJResult IJImageTranslator::YCbCrCompSplit(IJYCbCrImage444*	input, std::vector<uint8_t>& yComp
										   						 , std::vector<uint8_t>& bComp
										   						 , std::vector<uint8_t>& rComp)
{
	IJResult result = IJResult::Success;
	dbg__profileBlock("YCbCr components split");

	for (size_t i = 0; i < input->GetImageSize() / IJYCbCrPixel444::k_compCount; i++)
	{
		IJYCbCrPixel444* pixel = static_cast<IJYCbCrPixel444*>(input->GetPixelData()[i]);
		assert(pixel);
		if (pixel != nullptr)
		{
			yComp.push_back((*pixel)[0]);
			bComp.push_back((*pixel)[1]);
			rComp.push_back((*pixel)[2]);
		}
	}

	return result;
}

IJResult IJImageTranslator::RGBToYCbCr442(IJRGBImage* input, IJYCbCrImage422* output)
{
	return IJResult::Success;
}

IJResult IJImageTranslator::YCbCr444ToRGB(IJYCbCrImage444* input, IJRGBImage* output)
{
	return IJResult::Success;
}

IJResult IJImageTranslator::YCbCr422ToRGB(IJYCbCrImage422* input, IJRGBImage* output)
{
	return IJResult::Success;
}
