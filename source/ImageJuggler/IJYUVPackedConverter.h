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

		~context_rgb2yuv()
		{
			pRgb = nullptr;
			pY = nullptr;
			ySize = nullptr;
			uvSize = nullptr;
		}
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
		unsigned short* W;
		unsigned short* H;

		float packedW;
		float* pCoefs;
		unsigned coefRange;
		float pixelRadius;
		
		context_yuv2rgb()
			: pY(nullptr)
			, pU(nullptr)
			, pV(nullptr)
			, ySize(0)
			, uvSize(0)
			, packRate(0)
			, pRgb(nullptr)
			, W(nullptr)
			, H(nullptr)
			, coefRange(0)
			, pCoefs(nullptr)
			, pixelRadius(0)
		{}

		~context_yuv2rgb()
		{
			pY = nullptr;
			pU = nullptr;
			pV = nullptr;
			pRgb = nullptr;
			W = nullptr;
			H = nullptr;
			pCoefs = nullptr;
		}
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
	static void  deallocate_coefs_memory(context_yuv2rgb* ctx);
	static float calculate_coef_distance(float _X, float _Y);
	static void  calculate_coef(float* coef, float _X, float _Y);
	static void  calculate_coefs_upsample_range(context_yuv2rgb* ctx, float pixelRadius);
	static void  calculate_coefs_upsample(context_yuv2rgb* ctx);
	static float get_coef(context_yuv2rgb* ctx, int _X, int _Y);
	static void  calculate_pixel_upsample(context_yuv2rgb* ctx, int pux, int puy);

	static int   unpack(context_yuv2rgb* ctx);
	static int   unpack(const unsigned char* pY, const unsigned char* pU, const unsigned char* pV, 
						unsigned ySize, unsigned uvSize, unsigned packRate, 
						unsigned char* pRgb, unsigned short* W, unsigned short* H);
	static int   unpack(IJPackedColourImage* packedImage, IJRGBImage* rgbImage);

private:

	static float get_dither_coef(context_rgb2yuv* ctx, size_t idx);
	static void dither(context_rgb2yuv* ctx, pixel target, pixel pxlR, pixel pxlLL, 
					   pixel pxlL, pixel pxlLR);

};
