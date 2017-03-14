#pragma once

#include "IJImageInterface.h"

/* 
		IJPixel definition
*/

using comp_8_t = uint8_t;

class IJPixel
	: public IJPixelInterface<comp_8_t, 3>
{
public:
	using CompData_t = std::array<Comp_t, k_compCount>;

public:
	IJPixel() = default;
	IJPixel(const IJPixel& rhs) = default;
	IJPixel& operator=(const IJPixel& rhs) = default;

	IJPixel(IJPixel&& rhs) = delete;
	IJPixel& operator=(IJPixel&& rhs) = delete;

	virtual ~IJPixel() {}

	virtual std::vector<Comp_t> serialize() const override;
	virtual void deserialize(const std::array<Comp_t, k_compCount>& rawPixel);

	virtual Comp_t& operator [] (size_t compIdx) override;

private:
	CompData_t	m_comps;
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

	IJPixelAlpha(IJPixelAlpha&& rhs) = delete;
	IJPixelAlpha& operator = (IJPixelAlpha&& rhs) = delete;

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
	: public IJImageInterface<comp_8_t, IJPixel::k_compCount>
{
public:
	using PixelComp_t	= comp_8_t;
	using Pixel_t		= IJPixelInterface<PixelComp_t, IJPixel::k_compCount>;
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

	using IJImageInterface<comp_8_t, IJPixel::k_compCount>::GetPixelData;
	using IJImageInterface<comp_8_t, IJPixel::k_compCount>::Load;
	using IJImageInterface<comp_8_t, IJPixel::k_compCount>::Save;
};


/*
		IJImageAlpha definition
*/

class IJImageAlpha
	: IJImageInterface<comp_8_t, IJPixelAlpha::k_compCount>
{
public:
	using PixelComp_t		= comp_8_t;
	using Pixel_t			= IJPixelInterface<PixelComp_t, IJPixelAlpha::k_compCount>;
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

	using IJImageInterface<comp_8_t, IJPixelAlpha::k_compCount>::GetPixelData;
	using IJImageInterface<comp_8_t, IJPixelAlpha::k_compCount>::Load;
	using IJImageInterface<comp_8_t, IJPixelAlpha::k_compCount>::Save;
};