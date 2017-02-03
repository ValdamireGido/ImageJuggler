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
			// pushing new pixel
			output->AddPixel(ybrPixel);

			// translatign the rgb -> ycbcr
			(*ybrPixel)[0] = uint8_t(16  + ( 65.481 * (*rgbPixel)[0]  + 128.553 * (*rgbPixel)[1]  + 24.966 * (*rgbPixel)[2]));
			(*ybrPixel)[1] = uint8_t(128 + (-37.797 * (*rgbPixel)[0]  +  74.203 * (*rgbPixel)[1]  +  112.0 * (*rgbPixel)[2]));
			(*ybrPixel)[2] = uint8_t(128 + (  112.0 * (*rgbPixel)[0]  +  18.214 * (*rgbPixel)[1]  + 18.214 * (*rgbPixel)[2]));

			// increasing size 
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
