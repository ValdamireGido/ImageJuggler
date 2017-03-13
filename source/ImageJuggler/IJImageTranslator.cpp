#include "IJImageTranslator.h"
#include "Image/IJRGBImage.h"
#include "Image/IJYCbCrImage444.h"
#include "Image/IJYCbCrImage422.h"

#include <iostream>
#include <iomanip>

#include "IJUtils.h"

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

#if defined(RGB_TO_YCBCR_CONVERSION__GENERAL_KOEF) || defined(YCBCR_TO_RGB_CONVERSION__GENERAL_KOEF)
	#if IMAGE_CONVERSION_STANDARD_REC_601_BT_601
		static constexpr double s_koefR = 0.299;
		static constexpr double s_koefG = 0.587;
		static constexpr double s_koefB = 0.114;
	#elif IMAGE_CONVERSION_STANDARD_REC_709_BT_709
		static constexpr double s_koefR = 0.2126;
		static constexpr double s_koefG = 0.7152;
		static constexpr double s_koefB = 0.0722;
	#endif
#endif

//////////////////////////////////////////////////////////////////////////

std::array<uint8_t, 3> IJImageTranslator::TranslateRGBPixelToYBR(const std::vector<uint8_t>& rgbPixel)
{
	double  R = rgbPixel[0], 
			G = rgbPixel[1], 
			B = rgbPixel[2];
	std::array<uint8_t, 3> ybrPixel;
#if IMAGE_CONVERSION_STANDARD_FULL_RANGE_VALUES
	ybrPixel[0] = uint8_t( 0.299 * R  + 0.587 * G  + 0.114 * B);
	ybrPixel[1] = uint8_t(-0.169 * R  - 0.331 * G  + 0.500 * B) + 128;
	ybrPixel[2] = uint8_t( 0.500 * R  - 0.419 * G  - 0.081 * B) + 128;
#elif IMAGE_CONVERSION_STANDARD_SDTV
	ybrPixel[0] = uint8_t( 0.257 * R  + 0.504 * G  + 0.098 * B + 16 );
	ybrPixel[1] = uint8_t(-0.148 * R  - 0.291 * G  + 0.439 * B + 128);
	ybrPixel[2] = uint8_t( 0.439 * R  - 0.368 * G  - 0.071 * B + 128);
#elif IMAGE_CONVERSION_STANDARD_HDTV
	ybrPixel[0] = static_cast<uint8_t>( cusmath::clamp<double>( 0.183 * R  + 0.614 * G  + 0.062 * B + 16 , 16, 235) );
	ybrPixel[1] = static_cast<uint8_t>( cusmath::clamp<double>(-0.101 * R  - 0.339 * G  + 0.439 * B + 128, 16, 240) );
	ybrPixel[2] = static_cast<uint8_t>( cusmath::clamp<double>( 0.439 * R  - 0.399 * G  - 0.040 * B + 128, 16, 240) );
#elif IMAGE_CONVERSION_CUSTOM_COCG
	ybrPixel[0] = uint8_t( 0.33 * R + 0.33 * G + 0.33 * B);
	ybrPixel[1] = uint8_t(			  0.5  * G + 0.5  * B);
	ybrPixel[2] = uint8_t( 0.5  * R + 0.5  * G);
#endif
	return ybrPixel;
}

//////////////////////////////////////////////////////////////////////////

std::array<uint8_t, 3> IJImageTranslator::TranslateYBRPixelToRGB(const std::vector<uint8_t>& ybrPixel)
{
	double  Y = ybrPixel[0], 
			B = ybrPixel[1], 
			R = ybrPixel[2];
	std::array<uint8_t, 3> rgbPixel;
#if IMAGE_CONVERSION_STANDARD_FULL_RANGE_VALUES
	uint8_t R_ = R - 128;
	uint8_t B_ = B - 128;
	rgbPixel[0] = uint8_t( Y				 + 1.400 * R_ );
	rgbPixel[1] = uint8_t( Y	- 0.343 * B_ - 0.711 * R_ );
	rgbPixel[2] = uint8_t( Y	+ 1.765 * B_			  );
#elif IMAGE_CONVERSION_STANDARD_SDTV
	int16_t Y_ = Y - 16,
			B_ = B - 128, 
			R_ = R - 128,
			kY = Y_ * 1.164;
	rgbPixel[0] = uint8_t(kY			  + 1.196 * R_);
	rgbPixel[1] = uint8_t(kY - 0.392 * B_ - 0.813 * R_);
	rgbPixel[2] = uint8_t(kY + 2.017 * B_			  );
#elif IMAGE_CONVERSION_STANDARD_HDTV
	double  Y_ = Y - 16, 
			B_ = B - 128, 
			R_ = R - 128, 
			kY = static_cast<double>(Y_ * 1.164);
	rgbPixel[0] = static_cast<uint8_t>( cusmath::clamp<double>(kY			   + 1.793 * R_, 0, 255) );
	rgbPixel[1] = static_cast<uint8_t>( cusmath::clamp<double>(kY - 0.213 * B_ - 0.533 * R_, 0, 255) );
	rgbPixel[2] = static_cast<uint8_t>( cusmath::clamp<double>(kY + 2.112 * B_			   , 0, 255) );
#endif
	return rgbPixel;
}

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateRGBPixelToYBR(IJRGBPixel* rgbPixel, IJYCbCrPixel444* ybrPixel)
{
	ASSERT_PTR_VOID(rgbPixel);
	ASSERT_PTR_VOID(ybrPixel);
	*ybrPixel = IJYCbCrPixel444(TranslateRGBPixelToYBR(*rgbPixel));
}

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateYBRPixelToRGB(IJYCbCrPixel444* ybrPixel, IJRGBPixel* rgbPixel)
{
	ASSERT_PTR_VOID(ybrPixel);
	ASSERT_PTR_VOID(rgbPixel);
	*rgbPixel = IJRGBPixel(TranslateYBRPixelToRGB(*ybrPixel));
}

//////////////////////////////////////////////////////////////////////////

IJResult IJImageTranslator::RGBToYCbCr444(IJRGBImage* input, IJYCbCrImage444* output)
{
	ASSERT_PTR(input);
	ASSERT_PTR(output);

	assert(input->GetSize() != 0);
	if (input->GetSize() == 0)
	{
		return IJResult::ImageIsEmpty;
	}

	_CopyImageAttributes<uint8_t>(input, output);
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

	return IJResult::Success;
}

//////////////////////////////////////////////////////////////////////////

IJResult IJImageTranslator::YCbCr444ToRGB(IJYCbCrImage444* input, IJRGBImage* output)
{
	ASSERT_PTR(input);
	ASSERT_PTR(output);

	assert(input->GetSize() != 0);
	if (input->GetSize() == 0)
	{
		return IJResult::ImageIsEmpty;
	}

	_CopyImageAttributes<uint8_t>(input, output);
	for (size_t i = 0, size = input->GetSize() / IJYCbCrPixel444::k_compCount; i < size; i++)
	{
		IJYCbCrPixel444* ybrPixel = static_cast<IJYCbCrPixel444*>(input->GetPixelData()[i]);
		assert(ybrPixel);
		if (!ybrPixel)
		{
			return IJResult::InvalidImageFormat;
		}

		IJRGBPixel* rgbPixel = new IJRGBPixel();
		assert(rgbPixel);
		if (rgbPixel)
		{
			TranslateYBRPixelToRGB(ybrPixel, rgbPixel);
			output->AddPixel(rgbPixel);
		}
	}

	return IJResult::Success;;
}

//////////////////////////////////////////////////////////////////////////

//IJResult IJImageTranslator::RGBToYCbCr442(IJRGBImage* input, IJYCbCrImage422* output)
//{
//	ASSERT_PTR(input);
//	ASSERT_PTR(output);
//	return IJResult::Success;
//}

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
