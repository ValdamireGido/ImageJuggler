#include "IJImage.h"
#include <assert.h>
#include <strstream>


/* 
		IJPixelAlpha relization
*/

std::vector<IJPixelAlpha::Comp_t> IJPixelAlpha::serialize() const
{
	std::vector<Comp_t> rawPixel;
	rawPixel.resize(compCount());
	std::copy(m_comps.begin(), m_comps.end(), rawPixel.begin());
	return rawPixel;
}

void IJPixelAlpha::deserialize(const std::array<Comp_t, k_compCount>& rawPixel)
{
	assert(!rawPixel.empty());
	std::copy(rawPixel.begin(), rawPixel.end(), m_comps.begin());
}

inline IJPixelAlpha::Comp_t& IJPixelAlpha::operator [] (size_t compIdx)
{
	return m_comps[compIdx];
}


/*
		IJImage realization
*/

IJImage::IJImage(IJImageType type)
	: IJImageInterface<IJPixel, IJPixel::k_compCount>(type)
{}

IJImage::IJImage(const std::string& fileName, IJImageType type)
	: IJImageInterface<IJPixel, IJPixel::k_compCount>(fileName, type)
{}

IJResult IJImage::Load(std::istream& iStream) 
{
	iStream.seekg(0, iStream.end);
	int streamSize = iStream.tellg();
	iStream.seekg(18, iStream.beg);

	size_t size = streamSize / IJPixel::k_compCount;
	Resize(size);
	iStream.read((char*)&GetData().front(), streamSize);
	return IJResult::Success;
}

IJResult IJImage::Load(const std::vector<PixelComp_t>& rawImage)
{
	assert(!rawImage.empty());
	assert(GetData().empty());

	std::strstream stream((char*)&rawImage.front(), rawImage.size(), std::ios::in);
	return Load(stream);
}

IJResult IJImage::Save(std::ostream& oStream) 
{
	if (GetData().empty())
	{
		return IJResult::ImageIsEmpty;
	}

	oStream.write((const char*)&GetData().front(), GetPixelCount() * IJPixel::k_compCount);
	return IJResult::Success;
}

IJResult IJImage::Save(std::vector<PixelComp_t>& rawImage)
{
	assert(!GetData().empty());
	if (GetData().empty())
	{
		return IJResult::ImageIsEmpty;
	}

	auto it = GetData().begin();
	while (it != GetData().end())
	{
		std::vector<PixelComp_t> rawPixel;
		rawPixel[0] = it->c1;
		rawPixel[1] = it->c2;
		rawPixel[2] = it->c3;
		rawImage.insert(rawImage.end(), rawPixel.begin(), rawPixel.end());
		++it;
	}

	return IJResult::Success;
}


/*
		IJImageAlpha realization
*/

IJResult IJImageAlpha::Load(std::istream& iStream)
{
	while (!iStream.eof())
	{
		std::array<PixelComp_t, IJPixelAlpha::k_compCount> rawPixel;
		iStream.read((char*)&rawPixel.front(), IJPixel::k_compCount);
		IJPixelAlpha pixel;
		pixel.deserialize(rawPixel);
		AddPixel(pixel);
	}

	return IJResult::Success;
}

IJResult IJImageAlpha::Load(const std::vector<PixelComp_t>& rawImage)
{
	assert(!rawImage.empty());
	assert(GetData().empty());

	std::strstream stream((char*)&rawImage.front(), rawImage.size(), std::ios::in);
	return Load(stream);
}

IJResult IJImageAlpha::Save(std::ostream& oStream)
{
	if (GetData().empty())
	{
		return IJResult::ImageIsEmpty;
	}

	for (size_t i = 0, size = GetData().size(); i < size; ++i)
	{
		std::vector<PixelComp_t> rawPixel = GetData()[i].serialize();
		oStream.write((char*)&rawPixel.front(), IJPixel::k_compCount);
	}
	return IJResult::Success;
}

IJResult IJImageAlpha::Save(std::vector<PixelComp_t>& rawImage)
{
	assert(!GetData().empty());
	if (GetData().empty())
	{
		return IJResult::ImageIsEmpty;
	}

	auto it = GetData().begin();
	while (it != GetData().end())
	{
		std::vector<PixelComp_t> rawPixel = (*it).serialize();
		rawImage.insert(rawImage.end(), rawPixel.begin(), rawPixel.end());
		++it;
	}

	return IJResult::Success;
}
