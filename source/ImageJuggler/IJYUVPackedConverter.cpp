#include "IJYUVPackedConverter.h"
#include "Image/IJRGBImage.h"
#include "Image/IJPackedColourImage.h"
#include "IJImageTranslator.h"

#define IJUTILS_ENABLE_SPECIAL_LOGGER 0
#include "IJUtils.h"

#include "Fixed.h"

#include <cmath>
#include <algorithm>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#define PACKED_CONVERTER_USING_PARALLEL_UPSAMPLE     1
#define PACKED_CONVERTER_USING_CONSISTENTLY_UPSAMPLE 0

#define PACKED_CONVERTER_USING_CROSS_BICUBIC_FILTER 1
	#define PACKED_CONVERTER_USING_BICUBIC_FILTER_ELEMENTS 0

#define PACKED_CONVERTER_USING_BICUBIC_FILTER 0
	#define USE_BICUBIC_1 1
	#define USE_BICUBIC_2 0
	#define USE_BICUBIC_3 0

namespace 
{
	static float s_floyd_steinberg_keof[] = 
	{
		7/16, 3/16, 5/16, 1/16
	};
}

static std::ofstream s_log_ofstream;

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
//	pixel yuv;
//	yuv.y = ctx->pY;
//	yuv.u = py + pxlSize;
//	yuv.v = pu + pxlSize;
//	for (size_t pxlIdx = 0; pxlIdx < pxlSize; pxlIdx += ctx->chanels)
//	{
//		//pr = ctx->pRgb[pxlIdx];
//		//pg = *(&pr + 1);
//		//pb = *(&pg + 1);
//
////		dither(&pr, &pg, &pb);
//
//		IJImageTranslator::TranslateRGBToYUV(pr, pg, pb, *py, *pu, *pv);
//
//	}

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
	return 0;
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

		case filter_type::CubicBSpline:
			*coef = stbir__filter_cubic(_X, 0);

		default:break;
	}
}


void IJYuvPackedConverter::calculate_coef(context_yuv2rgb* ctx, float* coef, float _X, float _Y)
{
	float pxlIdx = calculate_coef_distance(_X, _Y);
	calculate_coef(ctx, coef, pxlIdx);
}


void IJYuvPackedConverter::calculate_coefs_upsample(context_yuv2rgb* ctx)
{
	ctx->pixelRadius = ctx->packRate*2;
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


inline float IJYuvPackedConverter::get_coef(context_yuv2rgb* ctx, int x, int y)
{
	int idx = (int)calculate_coef_distance((float)x, (float)y);
	return idx < ctx->coefsRange ? ctx->pCoefs[idx] : 0.f;
}


inline float IJYuvPackedConverter::get_coef(context_yuv2rgb* ctx, int coefIdx)
{
	return coefIdx < ctx->coefsRange ? ctx->pCoefs[coefIdx] : 0.f;
}


inline int IJYuvPackedConverter::get_upsample_idx(context_yuv2rgb* ctx, int upsample_x, int upsample_y)
{
	return upsample_x + upsample_y * (*ctx->W);
}


inline int IJYuvPackedConverter::get_downsample_idx(context_yuv2rgb* ctx, int downsample_x, int downsample_y)
{
	return downsample_x + downsample_y * ctx->packedW;
}


int IJYuvPackedConverter::unpack_consistently(context_yuv2rgb* ctx)
{
	ASSERT_PTR_INT(ctx);
	
	int upsample_idx = 0;
	int pixel_upsample_idx = 0;
	int pixel_downsample_idx = 0;
	unsigned char upsampled_u = 0;
	unsigned char upsampled_v = 0;

	*ctx->W = (unsigned short)sqrt(ctx->ySize);
	*ctx->H = *ctx->W;
	ctx->packedW = (int)sqrt(ctx->uvSize);
	calculate_coefs_upsample(ctx);
	for (int pixel_upsample_y = 0; pixel_upsample_y < *ctx->H; pixel_upsample_y++)
	{
		//IJ_SPECIAL_LOG_SCOPE_DURATION_FILE(s_log_ofstream, "Processing ling '%d'...", pixel_upsample_y);
		for (int pixel_upsample_x = 0; pixel_upsample_x < *ctx->W; pixel_upsample_x++)
		{
			// update pixel upsampled idx
			pixel_upsample_idx = get_upsample_idx(ctx, pixel_upsample_x, pixel_upsample_y);
			// update absolute upsample idx
			upsample_idx = pixel_upsample_idx * 3;

			IJ_SPECIAL_LOG_FILE(s_log_ofstream, "Pixel      - %d (%d, %d)", pixel_upsample_idx, pixel_upsample_x, pixel_upsample_y);
			IJ_SPECIAL_LOG_FILE(s_log_ofstream, "Downsample - %d (%d, %d)", pixel_upsample_x / ctx->packRate + pixel_upsample_y / ctx->packRate * ctx->packedW, 
								pixel_upsample_x / ctx->packRate, 
								pixel_upsample_y / ctx->packRate);

			// put Y comp to the R chanel
			ctx->pRgb[upsample_idx] = ctx->pY[pixel_upsample_idx];
#if PACKED_CONVERTER_USING_CROSS_BICUBIC_FILTER
			IJ_SPECIAL_LOG_FILE(s_log_ofstream, "\tHorizontal upsample:");
			// upsample horizontal
			for (int k = -2; k <= 2; k++)
			{
				int pixel_downsample_x = pixel_upsample_x / ctx->packRate + k;
				int pixel_downsample_y = pixel_upsample_y / ctx->packRate;
				pixel_downsample_idx = get_downsample_idx(ctx, pixel_downsample_x, pixel_downsample_y);
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
				IJ_SPECIAL_LOG_FILE(s_log_ofstream, "\t\tk = '%2d' didx = (%3d, %3d) coefIdx = '%2d' coef = '%2.4f'", k, pixel_downsample_x, pixel_downsample_y, coefIdx, coef);
			}

			IJ_SPECIAL_LOG_FILE(s_log_ofstream, "\tVertical upsample:");
			// upsample vertical
			upsampled_u = 0;
			upsampled_v = 0;
			for (int k = -2; k <= 2; k++)
			{
				int pixel_downsample_x = pixel_upsample_x / ctx->packRate;
				int pixel_downsample_y = pixel_upsample_y / ctx->packRate + k;
				pixel_downsample_idx = get_downsample_idx(ctx, pixel_downsample_x, pixel_downsample_y);
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
				upsampled_u += (unsigned char)(ctx->pU[pixel_downsample_idx] * coef);
				upsampled_v += (unsigned char)(ctx->pV[pixel_downsample_idx] * coef);
				IJ_SPECIAL_LOG_FILE(s_log_ofstream, "\t\tk = '%2d' didx = (%3d, %3d) coefIdx = '%2d' coef = '%2.4f'", k, pixel_downsample_x, pixel_downsample_y, coefIdx, coef);
			}

	#if PACKED_CONVERTER_USING_BICUBIC_FILTER_ELEMENTS 
			IJ_SPECIAL_LOG_FILE(s_log_ofstream, "\tBicubic upsample:");
			// upsample bicubic (also diagonal downsampled pixels)
			char bicubic_u = 0;
			char bicubic_v = 0;
			static const int k_bicubic_downsample_idx_offsets_count = 4;
			static const int k_downsample_idx_offsets[k_bicubic_downsample_idx_offsets_count][2] = { {-1, -1}, {-1, 1}, {1, 1}, {1, -1} };
			for (int k = 0; k < k_bicubic_downsample_idx_offsets_count; k++)
			{
				int pixel_downsample_x = pixel_upsample_x / ctx->packRate + k_downsample_idx_offsets[k][0];
				int pixel_downsample_y = pixel_upsample_y / ctx->packRate + k_downsample_idx_offsets[k][1];
				pixel_downsample_idx = get_downsample_idx(ctx, pixel_downsample_x, pixel_downsample_y);
				if (pixel_downsample_idx < 0 || pixel_downsample_idx >= ctx->uvSize)
				{
					continue;
				}

				int diff_x = abs(pixel_downsample_x*ctx->packRate - pixel_upsample_x);
				int diff_y = abs(pixel_downsample_y*ctx->packRate - pixel_upsample_y);
				float coef = get_coef(ctx, diff_x, diff_y);
				bicubic_u += (char)(ctx->pU[pixel_downsample_idx] * coef);
				bicubic_v += (char)(ctx->pV[pixel_downsample_idx] * coef);
				IJ_SPECIAL_LOG_FILE(s_log_ofstream, "\t\tk = '%2d' didx = (%3d, %3d) coefIdx (%3d,%3d) coef = '%2.4f'", k, pixel_downsample_x, pixel_downsample_y, diff_x, diff_y, coef);
			}
	#endif

	#if PACKED_CONVERTER_USING_BICUBIC_FILTER_ELEMENTS
			static const float horizontal_uv_weight = 0.5f;
			static const float vertical_uv_weight   = 0.5f;
			static const float bicubic_uv_weight    = 0.3f;
			ctx->pRgb[upsample_idx + 1] = (unsigned char)(ctx->pRgb[upsample_idx + 1] * horizontal_uv_weight + 
														  upsampled_u                 * vertical_uv_weight   + 
														  cusmath::clamp((float)bicubic_u, 16.f, 240.f)*bicubic_uv_weight);
			ctx->pRgb[upsample_idx + 2] = (unsigned char)(ctx->pRgb[upsample_idx + 2] * horizontal_uv_weight + 
														  upsampled_v                 * vertical_uv_weight   + 
														  cusmath::clamp((float)bicubic_v, 16.f, 240.f)*bicubic_uv_weight);
	#else
			ctx->pRgb[upsample_idx + 1] = (unsigned char)(ctx->pRgb[upsample_idx + 1]*0.5f + upsampled_u*0.5f);
			ctx->pRgb[upsample_idx + 2] = (unsigned char)(ctx->pRgb[upsample_idx + 2]*0.5f + upsampled_v*0.5f);
	#endif
#elif PACKED_CONVERTER_USING_BICUBIC_FILTER
			static const int k_bicubic_downsample_idx_offsets_count = 13;
			static const int k_bicubic_downsample_idx_offsets[k_bicubic_downsample_idx_offsets_count][2] = 
			{
						  		    { 0,  2},                             //     Donwsampled pixel that will be 
																		  // involved in the upsampled pixel
						  {-1,  1}, { 0,  1}, { 1,  1},					  // compilation with the separate weight 
																		  // for every calculated downsampled 
				{-2,  0}, {-1,  0}, { 0,  0}, { 1,  0}, { 2,  0},		  // pixel.
																		  //
				          {-1, -1}, { 0, -1}, { 1, -1}, 				  //
																		  //
						            { 0, -2}							  //
			};
			for (int k = 0; k < k_bicubic_downsample_idx_offsets_count; k++)
			{
				int pixel_downsample_x = pixel_upsample_x / ctx->packRate + k_bicubic_downsample_idx_offsets[k][0];
				int pixel_downsample_y = pixel_upsample_y / ctx->packRate + k_bicubic_downsample_idx_offsets[k][1];
				pixel_downsample_idx = get_downsample_idx(ctx, pixel_downsample_x, pixel_downsample_y);
				if (pixel_downsample_idx < 0 || pixel_downsample_idx > (ctx->uvSize))
				{
					continue;
				}

				int diff_x = abs(pixel_downsample_x*ctx->packRate - pixel_upsample_x);
				int diff_y = abs(pixel_downsample_y*ctx->packRate - pixel_upsample_y);
				float coef = get_coef(ctx, diff_x, diff_y);
				ctx->pRgb[upsample_idx + 1] += (unsigned char)(ctx->pU[pixel_downsample_idx] * coef);
				ctx->pRgb[upsample_idx + 2] += (unsigned char)(ctx->pV[pixel_downsample_idx] * coef);
				IJ_SPECIAL_LOG_FILE(s_log_ofstream, 
									"\t\tdownsampled (%2d, %2d), offset (%d, %d), diff (%2d, %2d) coef %.4f", 
									pixel_downsample_x, pixel_downsample_y, 
									k_bicubic_downsample_idx_offsets[k][0], k_bicubic_downsample_idx_offsets[k][1], 
									diff_x, diff_y, coef);
			}
#endif

			IJ_SPECIAL_LOG_FILE(s_log_ofstream, "\tTraslating pixel {%3d %3d %3d}", ctx->pRgb[upsample_idx + 0], 
																			        ctx->pRgb[upsample_idx + 1], 
																			        ctx->pRgb[upsample_idx + 2]);
			
			ijTranslateYuv2RgbComps(ctx->pRgb[upsample_idx + 0], ctx->pRgb[upsample_idx + 1], ctx->pRgb[upsample_idx + 2],
				ctx->pRgb[upsample_idx + 0], ctx->pRgb[upsample_idx + 1], ctx->pRgb[upsample_idx + 2]);

			IJ_SPECIAL_LOG_FILE(s_log_ofstream, "\tTranslated pixel {%3d %3d %3d}", ctx->pRgb[upsample_idx + 0], 
																			        ctx->pRgb[upsample_idx + 1], 
																			        ctx->pRgb[upsample_idx + 2]);
		}
	}

	deallocate_coefs_memory(ctx);

	return 0;
}


int IJYuvPackedConverter::unpack_parallel(context_yuv2rgb* ctx)
{
	ASSERT_PTR_INT(ctx);

	system("pause");

	int upsampled_idx = 0;
	int pixel_upsampled_idx = 0;
	int uv_upsampled_size = ctx->ySize;
	int w_downsampled = (int)sqrt(ctx->uvSize);
	int w_upsampled = (int)sqrt(uv_upsampled_size);
	const unsigned char* pu_downsampled = ctx->pU;
	const unsigned char* pv_downsampled = ctx->pV;
	unsigned char* pu_upsampled = (unsigned char*)malloc((size_t)uv_upsampled_size);
	unsigned char* pv_upsampled = (unsigned char*)malloc((size_t)uv_upsampled_size);
	
	system("pause");

	int ures = stbir__resize_arbitrary(nullptr, 
		pu_downsampled, w_downsampled, w_downsampled, 0,
		pu_upsampled, w_upsampled, w_upsampled, 0, 
		0, 0, 1, 1, nullptr, 1, -1, 0, 
		STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT, 
		STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);

	system("pause");

	int vres = stbir__resize_arbitrary(nullptr, 
		pv_downsampled, w_downsampled, w_downsampled, 0, 
		pv_upsampled, w_upsampled, w_upsampled, 0, 
		0, 0, 1, 1, nullptr, 1, -1, 0, 
		STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT, 
		STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);

	for (; pixel_upsampled_idx < ctx->ySize; pixel_upsampled_idx++)
	{
		upsampled_idx = pixel_upsampled_idx * 3;
		ijTranslateYuv2RgbComps(ctx->pY[pixel_upsampled_idx], pu_upsampled[pixel_upsampled_idx], pv_upsampled[pixel_upsampled_idx],
			ctx->pRgb[upsampled_idx + 0], ctx->pRgb[upsampled_idx + 1], ctx->pRgb[upsampled_idx + 2]);
	}

	*ctx->W = w_upsampled;
	*ctx->H = w_upsampled;
	free(pu_upsampled);
	free(pv_upsampled);

	system("pause");

	return 0;
}


int IJYuvPackedConverter::unpack(const unsigned char* pY, const unsigned char* pU, const unsigned char* pV,
								 unsigned ySize, unsigned uvSize, unsigned packRate,
								 unsigned char* pRgb, unsigned short* W, unsigned short* H)
{
	{
		s_log_ofstream.open("output/debug_output_log.dbglg", std::ios::out);
		IJ_SPECIAL_LOG_FILE(s_log_ofstream, "Starting unpack...");
	}

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

	int result = 0;
#if PACKED_CONVERTER_USING_PARALLEL_UPSAMPLE
	result = unpack_parallel(ctx);
#elif PACKED_CONVERTER_USING_CONSISTENTLY_UPSAMPLE
	result = unpack_consistently(ctx);
#endif

	delete ctx;
	ctx = nullptr;

	return result;
}


int IJYuvPackedConverter::unpack(IJPackedColourImage* packedImage, IJRGBImage* rgbImage)
{
	size_t size = rgbImage->GetSize();
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
