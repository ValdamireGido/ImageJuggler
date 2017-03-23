#include "IJYUVPackedConverter.h"
#include "Image/IJRGBImage.h"
#include "Image/IJPackedColourImage.h"
#include "IJImageTranslator.h"
#include "IJUtils.h"
//#include "Fixed.h"

#include <cmath>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

namespace 
{
	static float s_floyd_steinberg_keof[] = 
	{
		7/16, 3/16, 5/16, 1/16
	};
}

//
//float IJYuvPackedConverter::get_dither_coef(size_t idx, context_rgb2yuv* ctx)
//{
//	switch (ctx->ditherType)
//	{
//		case FloydSteinberg: return ::s_floyd_steinberg_keof[idx];
//		// ...
//		default:
//			assert(false);
//			return 0.f;
//	}	
//}
//
//
//void IJYuvPackedConverter::dither(pixel target, pixel pxlR, pixel pxlLL, pixel pxlL, pixel pxlLR, context_rgb2yuv* ctx)
//{
//	 uint8_t oldR = *target.r;
//	 uint8_t oldG = *target.g;
//	 uint8_t oldB = *target.b;
//     uint8_t newR = (oldR > 128) ? 255 : 0;
//	 uint8_t newG = (oldG > 128) ? 255 : 0;
//	 uint8_t newB = (oldB > 128) ? 255 : 0;
//     uint8_t rqe  = oldR - newR;
//
//     *pxlR.r = *pxlR.r + get_dither_coef(0, ctx) * rqe;
//	 *pxlR.g = *pxlR.g + get_dither_coef(0, ctx) * rqe;
//	 *pxlR.b = *pxlR.b + get_dither_coef(0, ctx) * rqe;
//
//	 //tmp->pixel(x-1,y+1) = tmp->pixel(x-1,y+1)  + 3.0/16 * propagationErreur ;
//	 //tmp->pixel(x,y+1) = tmp->pixel(x,y+1)  + 5.0/16 * propagationErreur ;
//	 //tmp->pixel(x+1,y+1) = tmp->pixel(x+1,y+1)  + 1.0/16 * propagationErreur ;
//
//}


int IJYuvPackedConverter::pack(context_rgb2yuv* ctx)
{
	size_t pxlSize = ctx->rgbW * ctx->rgbH;
	uint8_t pr = 0;
	uint8_t pg = 0; 
	uint8_t pb = 0;
	uint8_t* py = ctx->pPacked;
	uint8_t* pu = py + pxlSize;
	uint8_t* pv = pu + pxlSize;
	pixel yuv;
	yuv.y = ctx->pPacked;
	yuv.u = py + pxlSize;
	yuv.v = pu + pxlSize;
	for (size_t pxlIdx = 0; pxlIdx < pxlSize; pxlIdx += ctx->chanels)
	{
		//pr = ctx->pRgb[pxlIdx];
		//pg = *(&pr + 1);
		//pb = *(&pg + 1);

//		dither(&pr, &pg, &pb);

		IJImageTranslator::TranslateRGBToYUV(pr, pg, pb, *py, *pu, *pv);

	}

	return 0;
}


int IJYuvPackedConverter::pack(const unsigned char* rgbSrc, unsigned rgbSize, unsigned rgbW, unsigned rgbH,
							   unsigned char* packedImage, unsigned packRate, unsigned* ySize, unsigned* uvSize)
{
	context_rgb2yuv* ctx = new context_rgb2yuv();
	ctx->pRgb = rgbSrc;
	ctx->chanels = 3;
	ctx->rgbSize = rgbSize;
	ctx->rgbW = rgbW;
	ctx->rgbH = rgbH;
	ctx->pPacked = packedImage;
	ctx->packRate = packRate;
	ctx->ySize = ySize;
	ctx->uvSize = uvSize;
	int result = pack(ctx);

	delete ctx;
	ctx = nullptr;

	return result;
}


int IJYuvPackedConverter::pack(IJRGBImage* rgb, IJPackedColourImage* packedImage)
{
	const uint8_t* pRgb = &rgb->GetData().front().c1;
	size_t rgbSize = rgb->GetSize();
	size_t rgbW = rgb->GetWidth();
	size_t rgbH = rgb->GetHeight();
	uint8_t* pPacked = &packedImage->m_yImage->GetData()[0];
	size_t packRate = packedImage->GetPackRate();
	size_t* ySize = &packedImage->m_ySize;
	size_t* uvSize = &packedImage->m_uvSize;
	return pack(pRgb, rgbSize, rgbW, rgbH, 
				pPacked, packRate, ySize, uvSize);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////


void IJYuvPackedConverter::allocate_coefs_memory(context_yuv2rgb* ctx)
{
	ASSERT_PTR_VOID(ctx);
	ASSERT_PTR_VOID(!ctx->pCoefs);
	ctx->pCoefs = new float[ctx->coefRange];
}


float IJYuvPackedConverter::calculate_coef_distance(float _X, float _Y)
{
	return sqrtf(_X*_X + _Y*_Y); // usual distance to the point
}


void IJYuvPackedConverter::calculate_coef(float* coef, float _X, float _Y)
{
	float pxlIdx = calculate_coef_distance(_X, _Y);
	*coef = stbir__filter_mitchell(pxlIdx, 0);
}


void IJYuvPackedConverter::calculate_coefs_upsample_range(context_yuv2rgb* ctx, float pixelRadius)
{
	ctx->coefRange = (unsigned)fmin(ctx->packedW, pixelRadius*pixelRadius);
}


void IJYuvPackedConverter::calculate_coefs_upsample(context_yuv2rgb* ctx)
{
	ctx->pixelRadius = (float)ctx->packRate;
	calculate_coefs_upsample_range(ctx, 2*(ctx->pixelRadius+1));
	allocate_coefs_memory(ctx);
	unsigned coefIdx = 0;
	// coesf stored in range [-pixel radius: pixel radius) [-7; 6]
	for (int x = (int)-ctx->pixelRadius; x <= ctx->pixelRadius; x++)
	{
		for (int y = (int)-ctx->pixelRadius; y <= ctx->pixelRadius; y++)
		{
			calculate_coef(&ctx->pCoefs[coefIdx], (float)(x*2)/ctx->packRate, (float)(y*2)/ctx->packRate);
			++coefIdx;
		}
	}
}


void IJYuvPackedConverter::calculate_coef_idx(context_yuv2rgb* ctx, unsigned _pui, float* coefX, float* coefY)
{
	*coefY = (float)*ctx->W / ctx->coefRange;

	float pdi = (float)_pui / ctx->packRate;
	float coefIdx = (*coefX + ctx->pixelRadius + (2*ctx->pixelRadius + 1) * (*coefY + ctx->pixelRadius));

}


int IJYuvPackedConverter::unpack(context_yuv2rgb* ctx)
{
	ASSERT_PTR_INT(ctx);
	
	
	// pdi - pixel downsampled index
	// pui - pixel upsampled index
	int pui = 0;
	int pdi = 0;
	unsigned char* pOut = ctx->pRgb;
	unsigned char* pYIn = (unsigned char*)ctx->pPacked;
	unsigned char* pUIn = pYIn + ctx->ySize;
	unsigned char* pVIn = pUIn + ctx->uvSize;
	float W = *ctx->W = sqrtf((float)ctx->ySize);
	*ctx->H = *ctx->W;
	ctx->packedW = sqrtf((float)ctx->uvSize);

	int pdy = 0;
	int pdx = 0;
	calculate_coefs_upsample(ctx);
	for (int puy = 0; puy < *ctx->H; puy++)
	{
		pdy = puy/ctx->packRate;
		for (int pux = 0; pux < *ctx->W; pux++)
		{
			pui = pux + puy * (*ctx->W);
			pdx = pux / ctx->packRate;
			pdi = pdx + pdy * ctx->packedW;


			// coef downsampled idx x, y
			int cdx = pdx * ctx->packRate;
			int cdy = pdy * ctx->packRate;

		}
	}

	return 0;
}


int IJYuvPackedConverter::unpack(const unsigned char* pPacked, unsigned ySize, unsigned uvSize, unsigned packRate,
								 unsigned char* pRgb, unsigned* W, unsigned* H, unsigned* rgbSize)
{
	context_yuv2rgb* ctx = new context_yuv2rgb();
	ctx->pPacked = pPacked;
	ctx->ySize = ySize;
	ctx->uvSize = uvSize;
	ctx->packRate = packRate;
	ctx->pRgb = pRgb;
	ctx->W = W;
	ctx->H = H;
	ctx->rgbSize = rgbSize;

	int result = unpack(ctx);

	delete ctx;
	ctx = nullptr;

	return result;
}


int IJYuvPackedConverter::unpack(IJPackedColourImage* packedImage, IJRGBImage* rgbImage)
{
	int size = rgbImage->GetSize();
	if (!size)
	{
		rgbImage->Resize(packedImage->m_ySize);
	}
	return unpack((const unsigned char*)&packedImage->m_yImage->GetData().front(), (unsigned)packedImage->m_ySize, (unsigned)packedImage->m_uvSize, (unsigned)packedImage->GetPackRate(), 
		(unsigned char*)&rgbImage->GetData().front().c1, (unsigned*)&rgbImage->m_header.width, (unsigned*)&rgbImage->m_header.height, (unsigned*)&size);
}
