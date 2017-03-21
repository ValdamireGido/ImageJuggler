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

class IJPixelAlpha
	: public IJPixelInterface<comp_8_t, 4>
{
public:
	using CompData_t = std::array<Comp_t, k_compCount>;

public:
	IJPixelAlpha() = default;
	IJPixelAlpha(const IJPixelAlpha& rhs) = default;
	IJPixelAlpha& operator = (const IJPixelAlpha& rhs) = default;

	IJPixelAlpha(IJPixelAlpha&& rhs) = default;
	IJPixelAlpha& operator = (IJPixelAlpha&& rhs) = default;

	virtual ~IJPixelAlpha() {}

	virtual std::vector<Comp_t> serialize() const override;
	virtual void deserialize(const std::array<Comp_t, k_compCount>& rawPixel);
	virtual Comp_t& operator [] (size_t compIdx) override;

private:
	CompData_t m_comps;
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