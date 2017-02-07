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

#if defined(RGB_TO_YCBCR_CONVERSION__GENERAL_KOEF) || defined(YCBCR_TO_RGB_CONVERSION__GENERAL_KOEF)
static constexpr double s_koefR = 0.299;
static constexpr double s_koefG = 0.587;
static constexpr double s_koefB = 0.114;
#endif

//////////////////////////////////////////////////////////////////////////

std::array<uint8_t, 3> IJImageTranslator::TranslateRGBPixelToYBR(const std::vector<uint8_t>& rgbPixel)
{
#if defined(RGB_TO_YCBCR_CONVERSION__8BIT_SAMPLE)
	static constexpr double yrMu	=  65.481 / 256;
	static constexpr double ygMu	= 128.553 / 256;
	static constexpr double ybMu	=  25.064 / 256;

	static constexpr double brMu	= -37.738 / 256;
	static constexpr double bgMu	=  74.494 / 256;
	static constexpr double bbMu	= 112.439 / 256;

	static constexpr double rrMu	=  bbMu;
	static constexpr double rgMu	=  94.154 / 256;
	static constexpr double rbMu	=  18.285 / 256;

	std::array<uint8_t, 3> ybrPixel;
	//                            R                     G                     B
	ybrPixel[0] = uint8_t(16  + ( yrMu * rgbPixel[0]  + ygMu * rgbPixel[1]  + ybMu * rgbPixel[2]));  // Y
	ybrPixel[1] = uint8_t(128 + ( brMu * rgbPixel[0]  + bgMu * rgbPixel[1]  + bbMu * rgbPixel[2]));  // B 
	ybrPixel[2] = uint8_t(128 + ( rrMu * rgbPixel[0]  + rgMu * rgbPixel[1]  + rbMu * rgbPixel[2]));  // R
	return ybrPixel;
#elif defined(RGB_TO_YCBCR_CONVERSION__GENERAL_KOEF)
	std::array<uint8_t, 3> ybrPixel;
	ybrPixel[0] = uint8_t(s_koefR * rgbPixel[0] + s_koefG * rgbPixel[1] + s_koefB * rgbPixel[2]);
	ybrPixel[1] = uint8_t((rgbPixel[2] - ybrPixel[0]) * (1 - s_koefB));
	ybrPixel[2] = uint8_t((rgbPixel[1] - ybrPixel[0]) * (1 - s_koefR));
	return ybrPixel;
#endif 
}

//////////////////////////////////////////////////////////////////////////

std::array<uint8_t, 3> IJImageTranslator::TranslateYBRPixelToRGB(const std::vector<uint8_t>& ybrPixel)
{
#if defined(YCBCR_TO_RGB_CONVERSION__GENERAL)
	static constexpr double rgbyMu  = 298.082 / 256;
	static constexpr double rrMu	= 408.583 / 256;
	static constexpr double gbMu	= 100.291 / 256;
	static constexpr double grMu	= 208.120 / 256;
	static constexpr double bbMu	= 516.412 / 256;

	std::array<uint8_t, 3> rgbPixel;
	const double yCompMu = rgbyMu * (ybrPixel[0]);
	//				      Y           B                       R
	rgbPixel[0] = uint8_t(yCompMu   					    + rrMu * (ybrPixel[2]) 	- 222.921);  // R
	rgbPixel[1] = uint8_t(yCompMu   - gbMu * (ybrPixel[1])  - grMu * (ybrPixel[2])	+ 135.576);  // G
	rgbPixel[2] = uint8_t(yCompMu   + bbMu * (ybrPixel[1])	  						- 276.836);  // B
	return rgbPixel;
#elif defined(YCBCR_TO_RGB_CONVERSION__NO_ROUNDING)
	static constexpr double rgbyMu  = 255 / 219;
	static constexpr double rrMu	= 255 / 112 * 0.701;
	static constexpr double gbMu	= (255 * 0.886 * 0.114 / 112) / 0.587;
	static constexpr double grMu	= (255 * 0.701 * 0.299 / 112) / 0.587;
	static constexpr double bbMu	= 255 / 112 * 0.886;

	std::array<uint8_t, 3> rgbPixel;
	const double yCompMu = rgbyMu * (ybrPixel[0] - 16);
	//				      Y           B                             R
	rgbPixel[0] = uint8_t(yCompMu   					          + rrMu * (ybrPixel[2] - 128));  // R
	rgbPixel[1] = uint8_t(yCompMu   - gbMu * (ybrPixel[1] - 128)  - grMu * (ybrPixel[2] - 128));  // G
	rgbPixel[2] = uint8_t(yCompMu   + bbMu * (ybrPixel[1] - 128)	  						  );  // B
	return rgbPixel;
#elif defined(YCBCR_TO_RGB_CONVERSION__GENERAL_KOEF)
	std::array<uint8_t, 3> rgbPixel;
	rgbPixel[0] = uint8_t(ybrPixel[0] + ybrPixel[2] * (1 - s_koefR));
	rgbPixel[1] = uint8_t(ybrPixel[0] - ybrPixel[2] * (1 - s_koefB) * s_koefB / s_koefG - ybrPixel[1] * (1 - s_koefR) * s_koefR / s_koefG);
	rgbPixel[2] = uint8_t(ybrPixel[0] + ybrPixel[1] * (1 - s_koefB));
	return rgbPixel;
#endif 
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
