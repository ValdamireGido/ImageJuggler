#include "IJImageTranslator.h"
#include "Image/IJRGBImage.h"
#include "Image/IJYCbCrImage444.h"
#include "Image/IJYCbCrImage422.h"

#define ASSERT_PTR(ptr)\
do {\
	assert(ptr);\
	if (!ptr) {\
		return IJResult::BadMemoryPointer;\
	}\
}while(false)

#define ASSERT_PTR_VOID(ptr)\
do {\
	assert(ptr);\
	if (!ptr) {\
		return;\
	}\
} while (false)

//////////////////////////////////////////////////////////////////////////

std::array<uint8_t, 3> IJImageTranslator::TranslateYBRPixelToRGB(const std::vector<uint8_t>& ybrPixel)
{
	std::array<uint8_t, 3> rgbPixel;
	rgbPixel[0] = uint8_t(298.082 * ybrPixel[0] / 256									+ 408.583 * ybrPixel[2]		- 222.921);
	rgbPixel[1] = uint8_t(298.082 * ybrPixel[0] / 256	- 100.291 * ybrPixel[1] / 256	- 208.12  * ybrPixel[2]		+ 135.576);
	rgbPixel[2] = uint8_t(298.082 * ybrPixel[0] / 256	+ 516.412 * ybrPixel[1] / 256								- 276.836);
	return rgbPixel;
}

//////////////////////////////////////////////////////////////////////////

std::array<uint8_t, 3> IJImageTranslator::TranslateRGBPixelToYBR(const std::vector<uint8_t>& rgbPixel)
{
	std::array<uint8_t, 3> ybrPixel;
	ybrPixel[0] = uint8_t(16  + ( 65.481 * rgbPixel[0]  + 128.553 * rgbPixel[1]  + 24.966 * rgbPixel[2]));
	ybrPixel[1] = uint8_t(128 + (-37.797 * rgbPixel[0]  +  74.203 * rgbPixel[1]  +  112.0 * rgbPixel[2]));
	ybrPixel[2] = uint8_t(128 + (  112.0 * rgbPixel[0]  +  18.214 * rgbPixel[1]  + 18.214 * rgbPixel[2]));
	return ybrPixel;
}

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateYBRPixelToRGB(IJYCbCrPixel444* ybrPixel, IJRGBPixel* rgbPixel)
{
	ASSERT_PTR_VOID(ybrPixel);
	ASSERT_PTR_VOID(rgbPixel);
	*rgbPixel = IJRGBPixel(TranslateYBRPixelToRGB(static_cast<std::vector<uint8_t> >(*ybrPixel)));
}

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateRGBPixelToYBR(IJRGBPixel* rgbPixel, IJYCbCrPixel444* ybrPixel)
{
	ASSERT_PTR_VOID(rgbPixel);
	ASSERT_PTR_VOID(ybrPixel);
	*ybrPixel = IJYCbCrPixel444(TranslateRGBPixelToYBR(static_cast<std::vector<uint8_t> >(*rgbPixel)));
}

//////////////////////////////////////////////////////////////////////////

IJResult IJImageTranslator::RGBToYCbCr444(IJRGBImage* input, IJYCbCrImage444* output)
{
	ASSERT_PTR(input);
	ASSERT_PTR(output);

	assert(input->GetSize() != 0);
	if (input->GetSize() == 0)
	{
		return IJResult::ImageIsEmtpy;
	}

	for (size_t idx = 0; idx < input->GetSize() / IJRGBPixel::k_compCount; idx++)
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
		}
	}

	_CopyImageAttributes<uint8_t>(input, output);
	return IJResult::Success;
}

//////////////////////////////////////////////////////////////////////////

//IJResult IJImageTranslator::RGBToYCbCr442(IJRGBImage* input, IJYCbCrImage422* output)
//{
//	ASSERT_PTR(input);
//	ASSERT_PTR(output);
//	return IJResult::Success;
//}

//////////////////////////////////////////////////////////////////////////

IJResult IJImageTranslator::YCbCr444ToRGB(IJYCbCrImage444* input, IJRGBImage* output)
{
	ASSERT_PTR(input);
	ASSERT_PTR(output);

	IJResult result = IJResult::UnknownResult;



	return result;
}

//////////////////////////////////////////////////////////////////////////

//IJResult IJImageTranslator::YCbCr422ToRGB(IJYCbCrImage422* input, IJRGBImage* output)
//{
//	ASSERT_PTR(input);
//	ASSERT_PTR(output);
//	return IJResult::Success;
//}

//////////////////////////////////////////////////////////////////////////

IJResult IJImageTranslator::YCbCrCompSplit(IJYCbCrImage444*	input, std::vector<uint8_t>& yComp
										   						 , std::vector<uint8_t>& bComp
										   						 , std::vector<uint8_t>& rComp)
{
	ASSERT_PTR(input);
	IJResult result = IJResult::UnknownResult;

	for (size_t i = 0, size = input->GetSize() / IJYCbCrPixel444::k_compCount; i < size; i++)
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

//////////////////////////////////////////////////////////////////////////

IJResult IJImageTranslator::YBRToRGBCompSlit(IJYCbCrImage444* input, std::vector<uint8_t>& yComp
																   , std::vector<uint8_t>& bComp
																   , std::vector<uint8_t>& rComp)
{
	ASSERT_PTR(input);
	for (size_t i = 0; i < input->GetPixelData().size(); i++)
	{
		const IJYCbCrPixel444* pixel = static_cast<IJYCbCrPixel444*>(input->GetPixelData()[i]);
		std::vector<uint8_t> rawPixel = static_cast<std::vector<uint8_t> >(*pixel);
				
		std::array<uint8_t, 3> ypixel = IJImageTranslator::TranslateYBRPixelToRGB( {rawPixel[0], 128		, 128} );
		yComp.insert(yComp.end(), ypixel.begin(), ypixel.end());

		std::array<uint8_t, 3> bpixel = IJImageTranslator::TranslateYBRPixelToRGB( {235		   , rawPixel[1], 128} );
		bComp.insert(bComp.end(), bpixel.begin(), bpixel.end());

		std::array<uint8_t, 3> rpixel = IJImageTranslator::TranslateYBRPixelToRGB( {235		   , 128		, rawPixel[2]} );
		rComp.insert(rComp.end(), rpixel.begin(), rpixel.end());
	}

	return IJResult::Success;
}

//////////////////////////////////////////////////////////////////////////

IJResult IJImageTranslator::YBRToRGBCompSlit(IJYCbCrImage444* input, IJRGBImage* yComp
																   , IJRGBImage* bComp
																   , IJRGBImage* rComp)
{
	ASSERT_PTR(input);
	ASSERT_PTR(yComp);
	ASSERT_PTR(bComp);
	ASSERT_PTR(rComp);

	IJResult result = IJResult::UnknownResult;
	std::vector<uint8_t> yCompVector, bCompVector, rCompVector;
	result = YBRToRGBCompSlit(input, yCompVector, bCompVector, rCompVector);
	if (result != IJResult::Success)
	{
		return result;
	}

	_CopyImageAttributes<uint8_t>(input, yComp);
	result = yComp->Load(yCompVector);

	_CopyImageAttributes<uint8_t>(input, bComp);
	result = bComp->Load(bCompVector);

	_CopyImageAttributes<uint8_t>(input, rComp);
	result = rComp->Load(rCompVector);

	return result;
}

//////////////////////////////////////////////////////////////////////////

template <typename _PixelCompTy>
void IJImageTranslator::_CopyImageAttributes(IJImage<_PixelCompTy>* input, IJImage<_PixelCompTy>* output)
{
	output->SetSize(input->GetSize());
	output->SetWidth(input->GetWidth());
	output->SetHeight(input->GetHeight());
	output->SetCompressionType(input->GetCompressionType());
}
