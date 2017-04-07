#pragma once

#include "IJTypes.h"
#include <vector>
#include <array>

#define TRANSLATOR_USING_FIXED_POINT_IMPLEMENTATION 1
#define IMAGE_CONVERSION_STANDARD_SDTV 0
#define IMAGE_CONVERSION_STANDARD_HDTV 1

/*******************************************************************************************************************\
 								  SDTV Fixed point translation decalration
\*******************************************************************************************************************/

#define ijTranslate_yuv2rgb_sdtv_inline_fixed_point(y, u, v, r, g, b)												\
do {																												\
	fixed_int Y_ = y - 16, 																							\
			  kY = Y_ * 1.164f,																						\
			  U_ = u - 128,																							\
			  V_ = v - 128;																							\
	r = cusmath::clamp<fixed_int>(kY + V_ * 1.196f, 0, 255).to_uint();												\
	g = cusmath::clamp<fixed_int>(kY + U_ * (-0.392f) + V_ * (-0.813f), 0, 255).to_uint();							\
	b = cusmath::clamp<fixed_int>(kY + U_ * 2.017f, 0, 255).to_uint();												\
} while(false)

#define ijTranslate_rgb2yuv_sdtv_inline_fixed_point(r, g, b, y, u, v)												\
do {																												\
	y = cusmath::clamp<fixed_int>(r * 0.257f + g * 0.504f + b * 0.098f + 16, 16, 235).to_uint();					\
	u = cusmath::clamp<fixed_int>(r * (-0.169f) + g * (-0.291f) + b * 0.439f + 128, 16, 240).to_uint();				\
	v = cusmath::clamp<fixed_int>(r * 0.439f + g * (-0.368f) + b * (-0.071f) + 128, 16, 240).to_uint();				\
} while(false)

/*******************************************************************************************************************\
									HDTV Fixed point translation decalration
\*******************************************************************************************************************/

#define ijTranslate_yuv2rgb_hdtv_inline_fixed_point_declare_constants(y, u, v)										\
static fixed_int rrk = 1.13983f,																					\
				 gbk = -0.39465f, grk = -0.58060f,																	\
				 bbk = 2.03211f,																					\
				 yMu = 1.05f, yDec = 16, uvDec = 128,																\
				 lower = 0, upper = 255

#define ijTranslate_yuv2rgb_hdtv_inline_fixed_point(y, u, v, r, g, b)												\
do {																												\
	ijTranslate_yuv2rgb_hdtv_inline_fixed_point_declare_constants(y, u, v);											\
	fixed_int kY = (y - yDec) * yMu,																				\
			  U_ = u - uvDec,																						\
			  V_ = v - uvDec;																						\
	r = cusmath::clamp<fixed_int>(kY + V_ * rrk, lower, upper).to_uint();											\
	g = cusmath::clamp<fixed_int>(kY + U_ * gbk + V_ * grk, lower, upper).to_uint();								\
	b = cusmath::clamp<fixed_int>(kY + U_ * bbk, lower, upper).to_uint();											\
} while(false)

#define ijTranslate_rgb2yuv_hdtv_inline_fixed_point_declare_constans()												\
static fixed_int yrk =  0.2126f , ygk =  0.7152f , ybk =  0.0722f, 													\
				 brk = -0.09991f, bgk = -0.33609f, bbk = 0.436f,													\
				 rrk = 0.615f, rgk = -0.55861f, rbk = -0.05639f,													\
				 incY = 16, incUV = 128, lower = 16, upperY = 235, upperUV = 240

#define ijTranslate_rgb2yuv_hdtv_inline_fixed_point(r, g, b, y, u, v)												\
do { 																												\
	ijTranslate_rgb2yuv_hdtv_inline_fixed_point_declare_constans();													\
	y = cusmath::clamp<fixed_int>(r * yrk + g * ygk + b * ybk + incY, lower, upperY).to_uint();						\
	u = cusmath::clamp<fixed_int>(r * brk + g * bgk + b * bbk + incUV, lower, upperUV).to_uint();					\
	v = cusmath::clamp<fixed_int>(r * rrk + g * rgk + b * rbk + incUV, lower, upperUV).to_uint();					\
} while(false)

/*******************************************************************************************************************\
										General defines to use declaration
\*******************************************************************************************************************/


#if IMAGE_CONVERSION_STANDARD_SDTV
#define ijTranslateYuv2RgbComps(y, u, v, r, g, b) ijTranslate_yuv2rgb_sdtv_inline_fixed_point(y, u, v, r, g, b)
#define ijTranslateRgb2YuvComps(r, g, b, y, u, v) ijTranslate_rgb2yuv_sdtv_inline_fixed_point(r, g, b, y, u, v)
#elif IMAGE_CONVERSION_STANDARD_HDTV
#define ijTranslateYuv2RgbComps(y, u, v, r, g, b) ijTranslate_yuv2rgb_hdtv_inline_fixed_point(y, u, v, r, g, b)
#define ijTranslateRgb2YuvComps(r, g, b, y, u, v) ijTranslate_rgb2yuv_hdtv_inline_fixed_point(r, g, b, y, u, v)
#endif

/*******************************************************************************************************************\
										 For implementation declaration
\*******************************************************************************************************************/


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
