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

struct IJPixel;
class IJYCbCrPixel888;
class IJRGBPixel;

struct IJImageTranslator
{
	static std::array<uint8_t, 3> TranslateRGBPixelToYBR(const std::vector<uint8_t>& rgbPixel);
	static std::array<uint8_t, 3> TranslateYBRPixelToRGB(const std::vector<uint8_t>& ybrPixel);

	static void TranslateRGBPixelToYBR(IJPixel& rgbPixel, IJPixel& ybrPixel);
	static void TranslateYBRPixelToRGB(IJPixel& ybrPixel, IJPixel& rgbPixel);

	static void TranslateRGBToYBRArray(std::vector<IJPixel>::iterator rgbbegin, std::vector<IJPixel>::iterator rgbend, 
									   std::vector<IJPixel>::iterator ybrbegin, std::vector<IJPixel>::iterator ybrend);
	static void TranslateYBRToRGBArray(std::vector<IJPixel>::iterator ybrbegin, std::vector<IJPixel>::iterator ybrend, 
									   std::vector<IJPixel>::iterator rgbbegin, std::vector<IJPixel>::iterator rgbend);

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