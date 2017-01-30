//#pragma once 
//
//#include "IJImage.h"
//
//#include <array>
//#include <assert.h>
//
//class IJRGBImage
//	: public IJImage<uint8_t>
//{
//public:
//	struct IJRGBPixel
//		: public IJImage::Pixel<uint8_t>
//	{
//		static constexpr uint32_t k_compCount = 3u;
//
//		using Comp_t = uint8_t; 
//		using CompData = std::array<Comp_t, static_cast<size_t>(k_compCount)>;
//
//		IJRGBPixel()									= default;
//		IJRGBPixel(const std::vector<uint8_t>&  _data );
//		IJRGBPixel(const CompData&				_data );
//		IJRGBPixel(		 IJRGBPixel&				  )	= delete;
//		IJRGBPixel(const IJRGBPixel&&			other );
//		IJRGBPixel& operator=(		IJRGBPixel&		  )	= delete;
//		IJRGBPixel& operator=(const IJRGBPixel&& other);
//
//		Comp_t		operator[](uint32_t compIdx)	const override;
//		uint32_t	compCount()						const override;
//
//		/** 3 component for pixel: 
//			R - 0, 
//			G - 1, 
//			B - 2 
//			8 bits per component*/
//		CompData data;
//	};
//
//	using PixelComp_t		= uint8_t;
//	using Pixel_t			= IJImage<PixelComp_t>::Pixel<PixelComp_t>;
//	using RGBPixel_t		= IJRGBPixel;
//
//public:
//	IJRGBImage() = default;
//
//	IJResult Load(const std::string& fileName)		override;
//	IJResult Save(const std::string& fileName)		override;
//	IJResult LoadRGB(const std::string& fileName);
//	IJResult SaveRGB(const std::string& fileName);
//};
//
//// Inline IJRGBPixel
//
//inline IJRGBImage::IJRGBPixel::Comp_t IJRGBImage::IJRGBPixel::operator[](uint32_t compIdx) const 
//{
//	return data[compIdx];
//}
//
//inline uint32_t IJRGBImage::IJRGBPixel::compCount() const 
//{
//	return k_compCount;
//}
