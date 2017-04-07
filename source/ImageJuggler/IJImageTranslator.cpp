#include "IJImageTranslator.h"
#include "Image/IJRGBImage.h"
#include "Image/IJYCbCrImage888.h"

#include <iostream>
#include <iomanip>

#include "IJUtils.h"

#define TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION 1
#if TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
#include "Fixed.h"
using fixed_int = numeric::Fixed<16, 16>;
#endif

#define TRANSLATOR_USING_THREADS 1
#if TRANSLATOR_USING_THREADS
#define TRANSLATOR_AVAILABLE_THREADS 8
#else 
#define TRANSLATOR_AVAILABLE_THREADS 0
#endif

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateRGBToYUV(uint8_t R, uint8_t G, uint8_t B, 
										  uint8_t& y, uint8_t& u, uint8_t& v)
{
#if TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
	ijTranslateRgb2YuvComps(R, G, B, y, u, v);
#else
	#if IMAGE_CONVERSION_STANDARD_SDTV
		y = static_cast<uint8_t>(cusmath::clamp<float>(0.257f * R + 0.504f * G + 0.098f * B + 16, 16, 235));
		u = static_cast<uint8_t>(cusmath::clamp<float>(-0.148f * R - 0.291f * G + 0.439f * B + 128, 16, 240));
		v = static_cast<uint8_t>(cusmath::clamp<float>(0.439f * R - 0.368f * G - 0.071f * B + 128, 16, 240));
	#elif IMAGE_CONVERSION_STANDARD_HDTV
		y = static_cast<uint8_t>(cusmath::clamp<float>(0.183f * R + 0.614f * G + 0.062f * B + 16, 16, 235));
		u = static_cast<uint8_t>(cusmath::clamp<float>(-0.101f * R - 0.339f * G + 0.439f * B + 128, 16, 240));
		v = static_cast<uint8_t>(cusmath::clamp<float>(0.439f * R - 0.399f * G - 0.040f * B + 128, 16, 240));
	#endif
#endif // TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
}

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateYUVToRGB(uint8_t Y, uint8_t U, uint8_t V, 
										  uint8_t& r, uint8_t& g, uint8_t& b)
{
#if TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
	ijTranslateYuv2RgbComps(Y, U, V, r, g, b);
#else 	
	#if IMAGE_CONVERSION_STANDARD_SDTV
		float Y_ = Y - 16,
			U_ = B - 128,
			V_ = R - 128,
			kY = Y_ * 1.164f;
		rgbPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 1.196f * V_, 0, 255));
		rgbPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(kY - 0.392f * U_ - 0.813f * V_, 0, 255));
		rgbPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 2.017f * U_, 0, 255));
	#elif IMAGE_CONVERSION_STANDARD_HDTV
		float Y_ = Y - 16,
			  U_ = B - 128,
			  V_ = R - 128,
			  kY = Y_ * 1.164f;
		rgbPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 1.793f * V_, 0, 255));
		rgbPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(kY - 0.213f * U_ - 0.533f * V_, 0, 255));
		rgbPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 2.112f * U_, 0, 255));
	#endif  
#endif // TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
}

//////////////////////////////////////////////////////////////////////////

std::array<uint8_t, 3> IJImageTranslator::TranslateRGBPixelToYUV(const std::vector<uint8_t>& rgb)
{
	std::array<uint8_t, 3> yuv;
	TranslateRGBToYUV(rgb[0], rgb[1], rgb[2], yuv[0], yuv[1], yuv[2]);
	return yuv;
}

//////////////////////////////////////////////////////////////////////////

std::array<uint8_t, 3> IJImageTranslator::TranslateYUVPixelToRGB(const std::vector<uint8_t>& yuv)
{
	std::array<uint8_t, 3> rgb;
	TranslateYUVToRGB(yuv[0], yuv[1], yuv[2], rgb[0], rgb[1], rgb[2]);
	return rgb;
}

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateRGBPixelToYUV(IJPixel& rgb, IJPixel& yuv)
{
	TranslateRGBToYUV(rgb.c1, rgb.c2, rgb.c3, yuv.c1, yuv.c2, yuv.c3);
}

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateYUVPixelToRGB(IJPixel& ybr, IJPixel& rgb)
{
	TranslateYUVToRGB(ybr.c1, ybr.c2, ybr.c3, rgb.c1, rgb.c2, rgb.c3);
}

//////////////////////////////////////////////////////////////////////////

IJResult IJImageTranslator::RGBToYCbCr888(IJRGBImage* input, IJYCbCrImage888* output)
{
	ASSERT_PTR(input);
	ASSERT_PTR(output);

	assert(input->GetSize() != 0);
	if (input->GetSize() == 0)
	{
		return IJResult::ImageIsEmpty;
	}

	_CopyImageAttributes(input, output);
	const size_t nPixels = input->GetPixelCount();
	output->Resize(nPixels);

#if TRANSLATOR_USING_THREADS
	std::function<void (int)> iteration = [input, output] (int _i) 
	{
		IJPixel& rgbPixel = input->GetData()[_i];
		IJPixel& ybrPixel = output->GetData()[_i];
		TranslateRGBPixelToYUV(rgbPixel, ybrPixel);
	};
	parallel::asyncForeach(0, (int)nPixels, iteration, TRANSLATOR_AVAILABLE_THREADS);
#else 
	for (size_t idx = 0; idx < nPixels; idx++)
	{
		IJPixel& rgbPixel = input->GetData()[idx];
		IJPixel& ybrPixel = output->GetData()[idx];
		TranslateRGBPixelToYUV(rgbPixel, ybrPixel);
	}
#endif // TRANSLATOR_USING_THREADS

	return IJResult::Success;
}

//////////////////////////////////////////////////////////////////////////

IJResult IJImageTranslator::YCbCr888ToRGB(IJYCbCrImage888* input, IJRGBImage* output)
{
	ASSERT_PTR(input);
	ASSERT_PTR(output);

	assert(input->GetSize() != 0);
	if (input->GetSize() == 0)
	{
		return IJResult::ImageIsEmpty;
	}

	_CopyImageAttributes(input, output);
	const size_t nPixels = input->GetPixelCount();
	output->Resize(nPixels);

#if TRANSLATOR_USING_THREADS
	std::function<void (int)> iteration = [input, output] (int _i)
	{
		IJPixel& ybrPixel = input->GetData()[_i];
		IJPixel& rgbPixel = output->GetData()[_i];
		TranslateYUVPixelToRGB(ybrPixel, rgbPixel);
	};
	parallel::asyncForeach(0, (int)nPixels, iteration, TRANSLATOR_AVAILABLE_THREADS);
#else
	for (size_t i = 0; i < nPixels; i++)
	{
		IJPixel& ybrPixel = input->GetData()[i];
		IJPixel& rgbPixel = output->GetData()[i];
		TranslateYUVPixelToRGB(ybrPixel, rgbPixel);
	}
#endif // TRANSLATOR_USING_THREADS

	return IJResult::Success;;
}

//////////////////////////////////////////////////////////////////////////

IJResult IJImageTranslator::YCbCrCompSplit(IJYCbCrImage888*	input, std::vector<uint8_t>& yComp
										   						 , std::vector<uint8_t>& bComp
										   						 , std::vector<uint8_t>& rComp)
{
	ASSERT_PTR(input);
	IJResult result = IJResult::UnknownResult;

	for (size_t i = 0, size = input->GetSize() / IJYCbCrPixel888::k_compCount; i < size; i++)
	{
		IJPixel& pixel = input->GetData()[i];
		yComp.push_back(pixel.c1);
		bComp.push_back(pixel.c2);
		rComp.push_back(pixel.c3);
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////

IJResult IJImageTranslator::YBRToRGBCompSlit(IJYCbCrImage888* input, std::vector<uint8_t>& yComp
																   , std::vector<uint8_t>& bComp
																   , std::vector<uint8_t>& rComp)
{
	ASSERT_PTR(input);
	for (size_t i = 0; i < input->GetData().size(); i++)
	{
		const IJPixel& pixel = input->GetData()[i];
		std::vector<uint8_t> rawPixel;
		rawPixel[0]	= pixel.c1;
		rawPixel[1]	= pixel.c2;
		rawPixel[2] = pixel.c3;
				
		std::array<uint8_t, 3> ypixel = IJImageTranslator::TranslateYUVPixelToRGB( {rawPixel[0], 128		, 128} );
		yComp.insert(yComp.end(), ypixel.begin(), ypixel.end());

		std::array<uint8_t, 3> bpixel = IJImageTranslator::TranslateYUVPixelToRGB( {235		   , rawPixel[1], 128} );
		bComp.insert(bComp.end(), bpixel.begin(), bpixel.end());

		std::array<uint8_t, 3> rpixel = IJImageTranslator::TranslateYUVPixelToRGB( {235		   , 128		, rawPixel[2]} );
		rComp.insert(rComp.end(), rpixel.begin(), rpixel.end());
	}

	return IJResult::Success;
}

//////////////////////////////////////////////////////////////////////////

template <typename _PixelCompTy, size_t _compsCount>
void IJImageTranslator::_CopyImageAttributes(IJImageInterface<_PixelCompTy, _compsCount>* input, 
											 IJImageInterface<_PixelCompTy, _compsCount>* output)
{
	output->SetWidth(input->GetWidth());
	output->SetHeight(input->GetHeight());
	output->SetCompressionType(input->GetCompressionType());
}
