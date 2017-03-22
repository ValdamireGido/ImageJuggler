#pragma once

#include "IJImageInterface.h"

/* 
		IJPixel definition
*/

using comp_8_t = uint8_t;

struct IJPixel
	//: public IJPixelInterface<comp_8_t, 3>
{
	uint8_t c1;
	uint8_t c2;
	uint8_t c3;
	static const size_t k_compCount = 3;
};


/*
		IJPixelAlpha definition
*/

struct IJPixelAlpha
{
	uint8_t c1;
	uint8_t c2; 
	uint8_t c3;
	uint8_t a;
	static const size_t k_compCount = 4;
};


/*
		IJImage definition
*/

class IJImage
	: public IJImageInterface<IJPixel, IJPixel::k_compCount>
{
public:
	using PixelComp_t	= comp_8_t;
	using IJPixelPtr_t	= IJPixel*;

public:
	IJImage() = default;
	IJImage(const IJImage& rhs) = default;
	IJImage& operator = (const IJImage& rhs) = default;

	IJImage(IJImage&& rhs) = delete;
	IJImage& operator = (IJImage&& rhs) = delete;

	IJImage(IJImageType type);
	IJImage(const std::string& fileNmae, IJImageType type);

	virtual ~IJImage() {}

	virtual IJResult Load(std::istream& iStream) override;
	virtual IJResult Save(std::ostream& oStream) override;

	virtual IJResult Load(const std::vector<PixelComp_t>& rawImage);
	virtual IJResult Save(		std::vector<PixelComp_t>& rawImage);

	using IJImageInterface<IJPixel, IJPixel::k_compCount>::GetData;
	using IJImageInterface<IJPixel, IJPixel::k_compCount>::Load;
	using IJImageInterface<IJPixel, IJPixel::k_compCount>::Save;
};


/*
		IJImageAlpha definition
*/

class IJImageAlpha
	: IJImageInterface<IJPixelAlpha, IJPixelAlpha::k_compCount>
{
public:
	using PixelComp_t		= comp_8_t;
	using IJPixelAlphaPtr_t	= IJPixelAlpha*;

public:
	IJImageAlpha() = default;
	IJImageAlpha(const IJImageAlpha& rhs) = default;
	IJImageAlpha& operator = (const IJImageAlpha& rhs) = default;

	IJImageAlpha(IJImageAlpha&& rhs) = delete;
	IJImageAlpha& operator=(IJImageAlpha&& rhs) = delete;

	virtual ~IJImageAlpha() {}

	virtual IJResult Load(std::istream& iStream) override;
	virtual IJResult Save(std::ostream& oStream) override;

	virtual IJResult Load(const std::vector<PixelComp_t>& rawImage);
	virtual IJResult Save(		std::vector<PixelComp_t>& rawImage);

	using IJImageInterface<IJPixelAlpha, IJPixelAlpha::k_compCount>::GetData;
	using IJImageInterface<IJPixelAlpha, IJPixelAlpha::k_compCount>::Load;
	using IJImageInterface<IJPixelAlpha, IJPixelAlpha::k_compCount>::Save;
};