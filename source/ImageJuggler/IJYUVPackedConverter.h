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

struct IJYuvPackedConverter
{
	enum dither_type
	{
		FloydSteinberg = 0
	};

	struct context_rgb2yuv
	{
		const unsigned char* pRgb;
		unsigned chanels;
		unsigned rgbSize;
		unsigned rgbW;
		unsigned rgbH;
		unsigned char* pY;
		unsigned packRate;
		unsigned* ySize;
		unsigned* uvSize;
		dither_type ditherType;
	};

	struct context_yuv2rgb
	{
		const unsigned char* pY;
		const unsigned char* pU;
		const unsigned char* pV;
		unsigned ySize;
		unsigned uvSize;
		unsigned packRate;
		unsigned char* pRgb;
		unsigned* W;
		unsigned* H;
		unsigned* rgbSize;

		float packedW;
		float* pCoefs;
		unsigned coefRange;
		float pixelRadius;
		
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
	static int pack(const unsigned char* rgbSrc, unsigned rgbSize, unsigned rgbW, unsigned rgbH,
					unsigned char* packedImage, unsigned packRate, unsigned* ySize, unsigned* uvSize);
	static int pack(IJRGBImage* rgb, IJPackedColourImage* packedImage);


	static void	 allocate_coefs_memory(context_yuv2rgb* ctx);
	static float calculate_coef_distance(float _X, float _Y);
	static void  calculate_coef(float* coef, float _X, float _Y);
	static void  calculate_coefs_upsample_range(context_yuv2rgb* ctx, float pixelRadius);
	static void  calculate_coefs_upsample(context_yuv2rgb* ctx);
	static void  calculate_coef_idx(context_yuv2rgb* ctx, unsigned _pui, float* coefX, float* coefY);
	static float get_coef(context_yuv2rgb* ctx, int _X, int _Y);
	static void  calculate_pixel_upsample(context_yuv2rgb* ctx, int pux, int puy);

	static int   unpack(context_yuv2rgb* ctx);
	static int   unpack(const unsigned char* pY, const unsigned char* pU, const unsigned char* pV, 
						unsigned ySize, unsigned uvSize, unsigned packRate, 
						unsigned char* pRgb, unsigned* W, unsigned* H, unsigned* rgbSize);
	static int   unpack(IJPackedColourImage* packedImage, IJRGBImage* rgbImage);

private:

	static float get_dither_coef(context_rgb2yuv* ctx, size_t idx);
	static void dither(context_rgb2yuv* ctx, pixel target, pixel pxlR, pixel pxlLL, 
					   pixel pxlL, pixel pxlLR);

};
