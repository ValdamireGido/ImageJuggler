#pragma once

#include "IJTypes.h"
#include "ImageJuggler_config.h"
#include <vector>

#if defined(RGB_TO_YCBCR_CONVERSION__GENERAL_KOEF) || defined(YCBCR_TO_RGB_CONVERSION__GENERAL_KOEF)
	#define	IMAGE_CONVERSION_STANDARD_FULL_RANGE_VALUES 0
	#define IMAGE_CONVERSION_STANDARD_SDTV 0
	#define IMAGE_CONVERSION_STANDARD_HDTV 1
	#define IMAGE_CONVERSION_CUSTOM_COCG 0
#endif

template <typename _PixelCompTy, size_t _compsCount> class IJImageInterface;

class IJRGBImage;
class IJYCbCrImage888;

class IJYCbCrPixel888;
class IJRGBPixel;

struct IJImageTranslator
{
	static std::array<uint8_t, 3> TranslateRGBPixelToYBR(const std::vector<uint8_t>& rgbPixel);
	static std::array<uint8_t, 3> TranslateYBRPixelToRGB(const std::vector<uint8_t>& ybrPixel);

	static void TranslateRGBPixelToYBR(IJRGBPixel*		rgbPixel, IJYCbCrPixel888* ybrPixel);
	static void TranslateYBRPixelToRGB(IJYCbCrPixel888* ybrPixel, IJRGBPixel*	   rgbPixel);

	/*static void TranslateRGBToYBRArray(IJRGBImage::PixelData_t::iterator rgbbegin, IJRGBImage::PixelData_t::iterator rgbend, 
									   IJYCbCrImage888::PixelData_t::iterator ybrbegin, IJYCbCrImage888::PixelData_t::iterator ybrend);
	static void TranslateYBRToRGBArray(IJYCbCrImage888::PixelData_t::iterator ybrbegin, IJYCbCrImage888::PixelData_t::iterator ybrend, 
									   IJRGBImage::PixelData_t::iterator rgbbegin, IJRGBImage::PixelData_t::iterator rgbend);*/

	static IJResult RGBToYCbCr888(IJRGBImage*		input, IJYCbCrImage888* output);
	static IJResult YCbCr888ToRGB(IJYCbCrImage888*	input, IJRGBImage*		output);

	static IJResult YCbCrCompSplit(IJYCbCrImage888*	input, std::vector<uint8_t>& yComp
														 , std::vector<uint8_t>& bComp
														 , std::vector<uint8_t>& rComp);

	static IJResult YBRToRGBCompSlit(IJYCbCrImage888* input, std::vector<uint8_t>& yComp
														   , std::vector<uint8_t>& bComp
														   , std::vector<uint8_t>& rComp);

private:
	template <typename _PixelCompTy, size_t _compsCount>
	static void _CopyImageAttributes(IJImageInterface<_PixelCompTy, _compsCount>* input, 
									 IJImageInterface<_PixelCompTy, _compsCount>* output);
};