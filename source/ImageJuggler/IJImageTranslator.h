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
	static void TranslateRGBToYUV(uint8_t r, uint8_t g, uint8_t b, 
								  uint8_t& y, uint8_t& u, uint8_t& v);
	static void TranslateYUVToRGB(uint8_t y, uint8_t u, uint8_t v, 
								  uint8_t& r, uint8_t& g, uint8_t& b);

	static std::array<uint8_t, 3> TranslateRGBPixelToYUV(const std::vector<uint8_t>& rgbPixel);
	static std::array<uint8_t, 3> TranslateYUVPixelToRGB(const std::vector<uint8_t>& ybrPixel);

	static void TranslateRGBPixelToYUV(IJPixel& rgbPixel, IJPixel& ybrPixel);
	static void TranslateYUVPixelToRGB(IJPixel& ybrPixel, IJPixel& rgbPixel);

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