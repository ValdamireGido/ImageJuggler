#pragma once 

#include "IJTypes.h"

namespace numeric
{
	template<size_t, size_t>
	class Fixed;
}
using fixed_int = numeric::Fixed<16, 16>;

class IJRGBImage;
class IJPackedColourImage;

struct IJYUVPackedConverter
{
	enum dither_type
	{
		FloydSteinberg = 0
	};

	struct context_rgb2yuv
	{
		const uint8_t* pRgb;
		size_t chanels;
		size_t rgbSize;
		size_t rgbW;
		size_t rgbH;
		uint8_t* pPacked;
		size_t packRate;
		size_t* ySize;
		size_t* uvSize;
		dither_type ditherType;
	};

	struct context_yuv2rgb
	{
		const uint8_t* pPacked;
		size_t ySize;
		size_t uvSize;
		size_t packRate;
		uint8_t* pRgb;
		size_t* W;
		size_t* H;
		size_t* rgbSize;
		//dither_type ditherType; // -- ???
	};

	struct pixel
	{
		union 
		{
			uint8_t* r;
			uint8_t* y;
		};

		union 
		{
			uint8_t* g;
			uint8_t* u;
		};

		union 
		{
			uint8_t* b;
			uint8_t* v;
		};

		pixel() = default;
		pixel(uint8_t* c1, uint8_t* c2, uint8_t* c3)
			: r(c1), g(c2), b(c3)
		{}

		pixel(uint8_t* firstComp)
			: r(firstComp), g(firstComp+1), b(g+1)
		{}
	};

	static int pack(context_rgb2yuv* ctx);

	static int pack(const uint8_t* rgbSrc, size_t rgbSize, size_t rgbW, size_t rgbH,
					uint8_t* packedImage, size_t packRate, size_t* ySize, size_t* uvSize);

	static int pack(IJRGBImage* rgb, IJPackedColourImage* packedImage);

	static int unpack(context_yuv2rgb* ctx);

	static int unpack(const uint8_t* pPacked, size_t ySize, size_t uvSize, size_t packRate, uint8_t* pRgb, size_t* W, size_t* H, size_t* rgbSize);

private:

	static float get_dither_coef(size_t idx, context_rgb2yuv* ctx);

	static void dither(pixel target, pixel pxlR, pixel pxlLL, pixel pxlL, pixel pxlLR, context_rgb2yuv* ctx);

};
