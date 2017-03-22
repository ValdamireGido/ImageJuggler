#include "IJYUVPackedConverter.h"
#include "Image/IJRGBImage.h"
#include "Image/IJPackedColourImage.h"
#include "IJImageTranslator.h"
#include "IJUtils.h"
#include "Fixed.h"

//#define STB_IMAGE_RESIZE_IMPLEMENTATION
//#include "stb_image_resize.h"

namespace 
{
	static float s_floyd_steinberg_keof[] = 
	{
		7/16, 3/16, 5/16, 1/16
	};
}


float IJYUVPackedConverter::get_dither_coef(size_t idx, context_rgb2yuv* ctx)
{
	switch (ctx->ditherType)
	{
		case FloydSteinberg: return ::s_floyd_steinberg_keof[idx];
		// ...
		default:
			assert(false);
			return 0.f;
	}	
}


void IJYUVPackedConverter::dither(pixel target, pixel pxlR, pixel pxlLL, pixel pxlL, pixel pxlLR, context_rgb2yuv* ctx)
{
	 uint8_t oldR = *target.r;
	 uint8_t oldG = *target.g;
	 uint8_t oldB = *target.b;
     uint8_t newR = (oldR > 128) ? 255 : 0;
	 uint8_t newG = (oldG > 128) ? 255 : 0;
	 uint8_t newB = (oldB > 128) ? 255 : 0;
     uint8_t rqe  = oldR - newR;

     *pxlR.r = *pxlR.r + get_dither_coef(0, ctx) * rqe;
	 *pxlR.g = *pxlR.g + get_dither_coef(0, ctx) * rqe;
	 *pxlR.b = *pxlR.b + get_dither_coef(0, ctx) * rqe;

	 //tmp->pixel(x-1,y+1) = tmp->pixel(x-1,y+1)  + 3.0/16 * propagationErreur ;
	 //tmp->pixel(x,y+1) = tmp->pixel(x,y+1)  + 5.0/16 * propagationErreur ;
	 //tmp->pixel(x+1,y+1) = tmp->pixel(x+1,y+1)  + 1.0/16 * propagationErreur ;

}


int IJYUVPackedConverter::pack(context_rgb2yuv* ctx)
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


int IJYUVPackedConverter::pack(const uint8_t* rgbSrc, size_t rgbSize, size_t rgbW, size_t rgbH,
							   uint8_t* packedImage, size_t packRate, size_t* ySize, size_t* uvSize)
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


int IJYUVPackedConverter::pack(IJRGBImage* rgb, IJPackedColourImage* packedImage)
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


int IJYUVPackedConverter::unpack(context_yuv2rgb* ctx)
{

}


int IJYUVPackedConverter::unpack(const uint8_t* pPacked, size_t ySize, size_t uvSize, size_t packRate,
								 uint8_t* pRgb, size_t* W, size_t* H, size_t* rgbSize)
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
