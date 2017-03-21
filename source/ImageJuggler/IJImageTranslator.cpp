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
#include <thread>
#endif

//////////////////////////////////////////////////////////////////////////

std::array<uint8_t, 3> IJImageTranslator::TranslateRGBPixelToYBR(const std::vector<uint8_t>& rgbPixel)
{
	std::array<uint8_t, 3> ybrPixel;
#if TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
		fixed_int R = rgbPixel[0], G = rgbPixel[1], B = rgbPixel[2];
	#if IMAGE_CONVERSION_STANDARD_SDTV
		ybrPixel[0] = cusmath::clamp<fixed_int>(R * 0.257f    + G * 0.504f    + B * 0.098f    + 16 , 16, 235).to_uint();
		ybrPixel[1] = cusmath::clamp<fixed_int>(R * (-0.169f) + G * (-0.291f) + B * 0.439f    + 128, 16, 240).to_uint();
		ybrPixel[2] = cusmath::clamp<fixed_int>(R * 0.439f    + G * (-0.368f) + B * (-0.071f) + 128, 16, 240).to_uint();
	#elif IMAGE_CONVERSION_STANDARD_HDTV
		static fixed_int yrk = 0.183f, ygk = 0.614f, ybk = 0.062f, 
						 brk = -0.101f, bgk = -0.399f, bbk = 0.439f, 
						 rrk = 0.439f, rgk = -0.399f, rbk = -0.040f, 
						incY = 16, incUV = 128, lower = 16, upperY = 235, upperUV = 240;
		ybrPixel[0] = cusmath::clamp<fixed_int>(R * yrk + G * ygk + B * ybk + incY , lower, upperY).to_uint();
		ybrPixel[1] = cusmath::clamp<fixed_int>(R * brk + G * bgk + B * bbk + incUV, lower, upperUV).to_uint();
		ybrPixel[2] = cusmath::clamp<fixed_int>(R * rrk + G * rgk + B * rbk + incUV, lower, upperUV).to_uint();
	#endif // IMAGE_CONVERSION_STANDRARD_SDTV
#else
		float R = rgbPixel[0],
			  G = rgbPixel[1],
			  B = rgbPixel[2];
	#if IMAGE_CONVERSION_STANDARD_FULL_RANGE_VALUES
		ybrPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>( 0.299f * R + 0.587f * G + 0.114f * B, 0, 255));
		ybrPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(-0.169f * R - 0.331f * G + 0.500f * B + 128, 0, 255));
		ybrPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>( 0.500f * R - 0.419f * G - 0.081f * B + 128, 0, 255));
	#elif IMAGE_CONVERSION_STANDARD_SDTV
		ybrPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(0.257f * R + 0.504f * G + 0.098f * B + 16, 16, 235));
		ybrPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(-0.148f * R - 0.291f * G + 0.439f * B + 128, 16, 240));
		ybrPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(0.439f * R - 0.368f * G - 0.071f * B + 128, 16, 240));
	#elif IMAGE_CONVERSION_STANDARD_HDTV
		ybrPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(0.183f * R + 0.614f * G + 0.062f * B + 16, 16, 235));
		ybrPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(-0.101f * R - 0.339f * G + 0.439f * B + 128, 16, 240));
		ybrPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(0.439f * R - 0.399f * G - 0.040f * B + 128, 16, 240));
	#elif IMAGE_CONVERSION_CUSTOM_COCG
		ybrPixel[0] = uint8_t(0.33 * R + 0.33 * G + 0.33 * B);
		ybrPixel[1] = uint8_t(0.5  * G + 0.5  * B);
		ybrPixel[2] = uint8_t(0.5  * R + 0.5  * G);
	#endif
#endif // TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
	return ybrPixel;
}

//////////////////////////////////////////////////////////////////////////

std::array<uint8_t, 3> IJImageTranslator::TranslateYBRPixelToRGB(const std::vector<uint8_t>& ybrPixel)
{
	std::array<uint8_t, 3> rgbPixel;

#if TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
		fixed_int Y = ybrPixel[0], B = ybrPixel[1], R = ybrPixel[2];
	#if IMAGE_CONVERSION_STANDARD_SDTV
		fixed_int Y_ = Y - 16, 
				  kY = Y_ * 1.164f, 
				  B_ = B - 128, 
				  R_ = R - 128;
		rgbPixel[0] = cusmath::clamp<fixed_int>(kY + R_ * 1.196f, 0, 255).to_uint();
		rgbPixel[1] = cusmath::clamp<fixed_int>(kY + B_ * (-0.392f) + R_ * (-0.813f), 0, 255).to_uint();
		rgbPixel[2] = cusmath::clamp<fixed_int>(kY + B_ * 2.017f, 0, 255).to_uint();
	#elif IMAGE_CONVERSION_STANDARD_HDTV
		static fixed_int rrk = 1.793f, 
						 gbk = -0.213f, grk = -0.533f, 
						 bbk = 2.112f, 
						lower = 0, upper = 255;
		fixed_int Y_ = Y - 16, 
				  kY = Y_ * 1.164f, 
				  B_ = B - 128, 
				  R_ = R - 128;
		rgbPixel[0] = cusmath::clamp<fixed_int>(kY + R_ * rrk, lower, upper).to_uint();
		rgbPixel[1] = cusmath::clamp<fixed_int>(kY + B_ * gbk + R_ * grk, lower, upper).to_uint();
		rgbPixel[2] = cusmath::clamp<fixed_int>(kY + B_ * bbk, lower, upper).to_uint();
	#endif
#else 
		float Y = ybrPixel[0] * 1.04f,
			B = ybrPixel[1],
			R = ybrPixel[2];
	
	#if IMAGE_CONVERSION_STANDARD_FULL_RANGE_VALUES
		double R_ = R - 128,
			B_ = B - 128;
		rgbPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(Y + 1.400f * R_, 0, 255));
		rgbPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(Y - 0.343f * B_ - 0.711f * R_, 0, 255));
		rgbPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(Y + 1.765f * B_, 0, 255));
	#elif IMAGE_CONVERSION_STANDARD_SDTV
		float Y_ = Y - 16,
			B_ = B - 128,
			R_ = R - 128,
			kY = Y_ * 1.164f;
		rgbPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 1.196f * R_, 0, 255));
		rgbPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(kY - 0.392f * B_ - 0.813f * R_, 0, 255));
		rgbPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 2.017f * B_, 0, 255));
	#elif IMAGE_CONVERSION_STANDARD_HDTV
		float Y_ = Y - 16,
			  B_ = B - 128,
			  R_ = R - 128,
			  kY = Y_ * 1.164f;
		rgbPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 1.793f * R_, 0, 255));
		rgbPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(kY - 0.213f * B_ - 0.533f * R_, 0, 255));
		rgbPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 2.112f * B_, 0, 255));
	#endif  
#endif // TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION

	return rgbPixel;
}

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateRGBPixelToYBR(IJPixel& rgbPixel, IJPixel& ybrPixel)
{
#if TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
		fixed_int R = rgbPixel.c1, G = rgbPixel.c2, B = rgbPixel.c3;
	#if IMAGE_CONVERSION_STANDARD_SDTV
		ybrPixel[0] = cusmath::clamp<fixed_int>(R * 0.257f    + G * 0.504f    + B * 0.098f    + 16 , 16, 235).to_uint();
		ybrPixel[1] = cusmath::clamp<fixed_int>(R * (-0.169f) + G * (-0.291f) + B * 0.439f    + 128, 16, 240).to_uint();
		ybrPixel[2] = cusmath::clamp<fixed_int>(R * 0.439f    + G * (-0.368f) + B * (-0.071f) + 128, 16, 240).to_uint();
	#elif IMAGE_CONVERSION_STANDARD_HDTV
		static fixed_int yrk =  0.2126f , ygk =  0.7152f , ybk =  0.0722f, 
						 brk = -0.09991f, bgk = -0.33609f, bbk =  0.436f, 
						 rrk =  0.615f  , rgk = -0.55861f, rbk = -0.05639f, 
						incY = 16, incUV = 128, lower = 16, upperY = 235, upperUV = 240;
		ybrPixel.c1 = cusmath::clamp<fixed_int>(R * yrk + G * ygk + B * ybk + incY , lower, upperY).to_uint();
		ybrPixel.c2 = cusmath::clamp<fixed_int>(R * brk + G * bgk + B * bbk + incUV, lower, upperUV).to_uint();
		ybrPixel.c3 = cusmath::clamp<fixed_int>(R * rrk + G * rgk + B * rbk + incUV, lower, upperUV).to_uint();
	#endif // IMAGE_CONVERSION_STANDRARD_SDTV
#else
		float R = rgbPixel[0],
			  G = rgbPixel[1],
			  B = rgbPixel[2];
	#if IMAGE_CONVERSION_STANDARD_FULL_RANGE_VALUES
		ybrPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>( 0.299f * R + 0.587f * G + 0.114f * B, 0, 255));
		ybrPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(-0.169f * R - 0.331f * G + 0.500f * B + 128, 0, 255));
		ybrPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>( 0.500f * R - 0.419f * G - 0.081f * B + 128, 0, 255));
	#elif IMAGE_CONVERSION_STANDARD_SDTV
		ybrPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(0.257f * R + 0.504f * G + 0.098f * B + 16, 16, 235));
		ybrPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(-0.148f * R - 0.291f * G + 0.439f * B + 128, 16, 240));
		ybrPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(0.439f * R - 0.368f * G - 0.071f * B + 128, 16, 240));
	#elif IMAGE_CONVERSION_STANDARD_HDTV
		ybrPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(0.183f * R + 0.614f * G + 0.062f * B + 16, 16, 235));
		ybrPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(-0.101f * R - 0.339f * G + 0.439f * B + 128, 16, 240));
		ybrPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(0.439f * R - 0.399f * G - 0.040f * B + 128, 16, 240));
	#elif IMAGE_CONVERSION_CUSTOM_COCG
		ybrPixel[0] = uint8_t(0.33 * R + 0.33 * G + 0.33 * B);
		ybrPixel[1] = uint8_t(0.5  * G + 0.5  * B);
		ybrPixel[2] = uint8_t(0.5  * R + 0.5  * G);
	#endif
#endif // TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
}

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateYBRPixelToRGB(IJPixel& ybrPixel, IJPixel& rgbPixel)
{
	#if TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
		fixed_int Y = ybrPixel.c1, B = ybrPixel.c2, R = ybrPixel.c3;
	#if IMAGE_CONVERSION_STANDARD_SDTV
		fixed_int Y_ = Y - 16, 
				  kY = Y_ * 1.164f, 
				  B_ = B - 128, 
				  R_ = R - 128;
		rgbPixel[0] = cusmath::clamp<fixed_int>(kY + R_ * 1.196f, 0, 255).to_uint();
		rgbPixel[1] = cusmath::clamp<fixed_int>(kY + B_ * (-0.392f) + R_ * (-0.813f), 0, 255).to_uint();
		rgbPixel[2] = cusmath::clamp<fixed_int>(kY + B_ * 2.017f, 0, 255).to_uint();
	#elif IMAGE_CONVERSION_STANDARD_HDTV
		static fixed_int rrk = 1.13983f, 
						 gbk = -0.39465f, grk = -0.58060f, 
						 bbk = 2.03211f, 
						yMu = 1.f, yDec = 16, uvDec = 128,
						lower = 0, upper = 255;
		fixed_int kY = (Y - yDec) * yMu, 
				  B_ = B - uvDec, 
				  R_ = R - uvDec;
		rgbPixel.c1 = cusmath::clamp<fixed_int>(kY + R_ * rrk, lower, upper).to_uint();
		rgbPixel.c2 = cusmath::clamp<fixed_int>(kY + B_ * gbk + R_ * grk, lower, upper).to_uint();
		rgbPixel.c3 = cusmath::clamp<fixed_int>(kY + B_ * bbk, lower, upper).to_uint();
	#endif
#else 
		float Y = ybrPixel[0] * 1.04f,
			B = ybrPixel[1],
			R = ybrPixel[2];
	
	#if IMAGE_CONVERSION_STANDARD_FULL_RANGE_VALUES
		double R_ = R - 128,
			B_ = B - 128;
		rgbPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(Y + 1.400f * R_, 0, 255));
		rgbPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(Y - 0.343f * B_ - 0.711f * R_, 0, 255));
		rgbPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(Y + 1.765f * B_, 0, 255));
	#elif IMAGE_CONVERSION_STANDARD_SDTV
		float Y_ = Y - 16,
			B_ = B - 128,
			R_ = R - 128,
			kY = Y_ * 1.164f;
		rgbPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 1.196f * R_, 0, 255));
		rgbPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(kY - 0.392f * B_ - 0.813f * R_, 0, 255));
		rgbPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 2.017f * B_, 0, 255));
	#elif IMAGE_CONVERSION_STANDARD_HDTV
		float Y_ = Y - 16,
			  B_ = B - 128,
			  R_ = R - 128,
			  kY = Y_ * 1.164f;
		rgbPixel[0] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 1.793f * R_, 0, 255));
		rgbPixel[1] = static_cast<uint8_t>(cusmath::clamp<float>(kY - 0.213f * B_ - 0.533f * R_, 0, 255));
		rgbPixel[2] = static_cast<uint8_t>(cusmath::clamp<float>(kY + 2.112f * B_, 0, 255));
	#endif  
#endif // TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION
}

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateRGBToYBRArray(IJRGBImage::PixelData_t::iterator rgbbegin, IJRGBImage::PixelData_t::iterator rgbend, IJYCbCrImage888::PixelData_t::iterator ybrbegin, IJYCbCrImage888::PixelData_t::iterator ybrend)
{

}

//////////////////////////////////////////////////////////////////////////

void IJImageTranslator::TranslateYBRToRGBArray(IJYCbCrImage888::PixelData_t::iterator ybrbegin, IJYCbCrImage888::PixelData_t::iterator ybrend, IJRGBImage::PixelData_t::iterator rgbbegin, IJRGBImage::PixelData_t::iterator rgbend)
{

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
		TranslateRGBPixelToYBR(rgbPixel, ybrPixel);
	};
	parallel::asyncForeach(0, nPixels, iteration, TRANSLATOR_AVAILABLE_THREADS);
#else 
	for (size_t idx = 0; idx < nPixels; idx++)
	{
		IJPixel& rgbPixel = input->GetData()[idx];
		IJPixel& ybrPixel = output->GetData()[idx];
		TranslateRGBPixelToYBR(rgbPixel, ybrPixel);
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
		TranslateYBRPixelToRGB(ybrPixel, rgbPixel);
	};
	parallel::asyncForeach(0, nPixels, iteration, TRANSLATOR_AVAILABLE_THREADS);
#else
	for (size_t i = 0; i < nPixels; i++)
	{
		IJPixel& ybrPixel = input->GetData()[i];
		IJPixel& rgbPixel = output->GetData()[i];
		TranslateYBRPixelToRGB(ybrPixel, rgbPixel);
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

template <typename _PixelCompTy, size_t _compsCount>
void IJImageTranslator::_CopyImageAttributes(IJImageInterface<_PixelCompTy, _compsCount>* input, 
											 IJImageInterface<_PixelCompTy, _compsCount>* output)
{
	output->SetWidth(input->GetWidth());
	output->SetHeight(input->GetHeight());
	output->SetCompressionType(input->GetCompressionType());
}
