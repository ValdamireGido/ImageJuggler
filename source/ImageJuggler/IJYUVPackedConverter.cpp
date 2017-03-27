#include "IJYUVPackedConverter.h"
#include "Image/IJRGBImage.h"
#include "Image/IJPackedColourImage.h"
#include "IJImageTranslator.h"
#include "IJUtils.h"
//#include "Fixed.h"

#include <cmath>
#include <algorithm>

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
	uint8_t* py = ctx->pY;
	uint8_t* pu = py + pxlSize;
	uint8_t* pv = pu + pxlSize;
	pixel yuv;
	yuv.y = ctx->pY;
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
	ctx->pY = packedImage;
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
	uint8_t* pY = &packedImage->m_yImage->GetData()[0];
	size_t packRate = packedImage->GetPackRate();
	size_t* ySize = &packedImage->m_ySize;
	size_t* uvSize = &packedImage->m_uvSize;
	return pack(pRgb, rgbSize, rgbW, rgbH, 
				pY, packRate, ySize, uvSize);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////


void IJYuvPackedConverter::allocate_coefs_memory(context_yuv2rgb* ctx)
{
	ASSERT_PTR_VOID(!ctx->pCoefs);
	ctx->pCoefs = new float[ctx->coefsRange];
}


void IJYuvPackedConverter::deallocate_coefs_memory(context_yuv2rgb* ctx)
{
	ASSERT_PTR_VOID(ctx->pCoefs);
	delete [] ctx->pCoefs;
}


float IJYuvPackedConverter::calculate_coef_distance(float _X, float _Y)
{
	return sqrtf(_X*_X + _Y*_Y); // usual distance to the point
}


void IJYuvPackedConverter::calculate_coef(context_yuv2rgb* ctx, float* coef, float _X)
{
	switch (ctx->filterType)
	{
		case filter_type::Mitchell: 
			*coef = stbir__filter_mitchell(_X, 0);
			break;

		case filter_type::Catmullrom:
			*coef = stbir__filter_catmullrom(_X, 0);
			break;

		default:break;
	}
}


void IJYuvPackedConverter::calculate_coef(context_yuv2rgb* ctx, float* coef, float _X, float _Y)
{
	float pxlIdx = calculate_coef_distance(_X, _Y);
	switch (ctx->filterType)
	{
		case filter_type::Mitchell:
			*coef = stbir__filter_mitchell(pxlIdx, 0);
			break;

		case filter_type::Catmullrom:
			*coef = stbir__filter_catmullrom(pxlIdx, 0);

		default: break;
	}
}


void IJYuvPackedConverter::calculate_coefs_upsample(context_yuv2rgb* ctx)
{
	ctx->pixelRadius = ctx->packRate<<1;
	ctx->coefsRange = ctx->pixelRadius;
	allocate_coefs_memory(ctx);
	unsigned coefIdx = 0;
	// coefs from nearest to furthest
	for (int y = 0; y < ctx->coefsRange; y++)
	{
		float distance = (float)y/(ctx->packRate);
		calculate_coef(ctx, &ctx->pCoefs[y], distance);
	}
}


float IJYuvPackedConverter::get_coef(context_yuv2rgb* ctx, int coefIdx)
{
	return coefIdx < ctx->coefsRange ? ctx->pCoefs[coefIdx] : 0.f;
}


int IJYuvPackedConverter::unpack(context_yuv2rgb* ctx)
{
	ASSERT_PTR_INT(ctx);
	
	int upsample_idx = 0;
	int pixel_upsample_idx = 0;
	int pixel_downsample_idx = 0;
	unsigned char u = 0, v = 0;

	*ctx->W = (unsigned short)sqrt(ctx->ySize);
	*ctx->H = *ctx->W;
	ctx->packedW = (int)sqrt(ctx->uvSize);
	calculate_coefs_upsample(ctx);
	for (int pixel_upsample_y = 0; pixel_upsample_y < *ctx->H; pixel_upsample_y++)
	{
		for (int pixel_upsample_x = 0; pixel_upsample_x < *ctx->W; pixel_upsample_x++)
		{
			// update pixel upsampled idx
			pixel_upsample_idx = pixel_upsample_x + pixel_upsample_y * (*ctx->W);
			// update absolute upsample idx
			upsample_idx = pixel_upsample_idx * 3;

			// put Y comp to the R chanel
			ctx->pRgb[upsample_idx] = ctx->pY[pixel_upsample_idx];

			// upsample horizontal
			for (int k = -2; k <= 2; k++)
			{
				int pixel_downsample_x = pixel_upsample_x / ctx->packRate + k;
				int pixel_downsample_y = pixel_upsample_y / ctx->packRate;
				pixel_downsample_idx = pixel_downsample_x + pixel_downsample_y * ctx->packedW;
				if (pixel_downsample_x < 0)
				{
					continue;
				}

				if (pixel_downsample_x >= ctx->packedW)
				{
					pixel_downsample_idx = pixel_upsample_x / ctx->packRate - k + pixel_downsample_y * ctx->packedW;
				}

				int coefIdx = abs(pixel_downsample_x*ctx->packRate - pixel_upsample_x);
				float coef = get_coef(ctx, coefIdx);
				ctx->pRgb[upsample_idx + 1] += (unsigned char)(ctx->pU[pixel_downsample_idx] * coef);
				ctx->pRgb[upsample_idx + 2] += (unsigned char)(ctx->pV[pixel_downsample_idx] * coef);
			}

			// upsample vertical
			u = 0;
			v = 0;
			for (int k = -2; k <= 2; k++)
			{
				int pixel_downsample_x = pixel_upsample_x / ctx->packRate;
				int pixel_downsample_y = pixel_upsample_y / ctx->packRate + k;
				pixel_downsample_idx = pixel_downsample_x + pixel_downsample_y * ctx->packedW;
				if (pixel_downsample_y  < 0)
				{
					continue;
				}

				if (pixel_downsample_y >= ctx->packedW)
				{
					pixel_downsample_idx = pixel_downsample_x + pixel_upsample_y / ctx->packRate * ctx->packedW - k;
				}

				int coefIdx = abs(pixel_downsample_y*ctx->packRate - pixel_upsample_y);
				float coef = get_coef(ctx, coefIdx);
				u += (unsigned char)(ctx->pU[pixel_downsample_idx] * coef);
				v += (unsigned char)(ctx->pV[pixel_downsample_idx] * coef);
			}

			ctx->pRgb[upsample_idx + 1] = (unsigned char)(ctx->pRgb[upsample_idx + 1]*0.5f + u*0.5f);
			ctx->pRgb[upsample_idx + 2] = (unsigned char)(ctx->pRgb[upsample_idx + 2]*0.5f + v*0.5f);

			IJImageTranslator::TranslateYUVToRGB(
				ctx->pRgb[upsample_idx + 0], 
				ctx->pRgb[upsample_idx + 1], 
				ctx->pRgb[upsample_idx + 2], 
				ctx->pRgb[upsample_idx + 0], 
				ctx->pRgb[upsample_idx + 1], 
				ctx->pRgb[upsample_idx + 2]);
		}
	}

	deallocate_coefs_memory(ctx);

	return 0;
}


int IJYuvPackedConverter::unpack(const unsigned char* pY, const unsigned char* pU, const unsigned char* pV,
								 unsigned ySize, unsigned uvSize, unsigned packRate,
								 unsigned char* pRgb, unsigned short* W, unsigned short* H)
{
	context_yuv2rgb* ctx = new context_yuv2rgb;
	ctx->pY = pY;
	ctx->pU = pU;
	ctx->pV = pV;
	ctx->ySize = ySize;
	ctx->uvSize = uvSize;
	ctx->packRate = packRate;
	ctx->filterType = filter_type::Mitchell;
	ctx->pRgb = pRgb;
	ctx->W = W;
	ctx->H = H;

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
	return unpack(
		(const unsigned char*)&packedImage->m_yImage->GetData().front(), 
		(const unsigned char*)&packedImage->m_uImage->GetData().front(),
		(const unsigned char*)&packedImage->m_vImage->GetData().front(),
		(int)packedImage->m_ySize, 
		(int)packedImage->m_uvSize, 
		(int)packedImage->GetPackRate(), 
		(unsigned char*)&rgbImage->GetData().front().c1, 
		(unsigned short*)&rgbImage->m_header.width, 
		(unsigned short*)&rgbImage->m_header.height);
}
