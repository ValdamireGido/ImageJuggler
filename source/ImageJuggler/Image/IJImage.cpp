#include "IJImage.h"
#include <assert.h>
#include <strstream>

/*
		IJPixel realization
*/

std::vector<IJPixel::Comp_t> IJPixel::serialize() const
{
	std::vector<Comp_t> rawPixel;
	rawPixel.resize(compCount());
	std::copy(m_comps.begin(), m_comps.end(), rawPixel.begin());
	return rawPixel;
}

void IJPixel::deserialize(const std::array<Comp_t, k_compCount>& rawPixel)
{
	assert(!rawPixel.empty());
	std::copy(rawPixel.begin(), rawPixel.end(), m_comps.begin());
}

inline IJPixel::Comp_t& IJPixel::operator [] (size_t compIdx)
{
	return m_comps[compIdx];
}


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
	: IJImageInterface<PixelComp_t, IJPixel::k_compCount>(type)
{}

IJImage::IJImage(const std::string& fileName, IJImageType type)
	: IJImageInterface<PixelComp_t, IJPixel::k_compCount>(fileName, type)
{}

IJResult IJImage::Load(std::istream& iStream) 
{
	_LoadHeader(iStream);
	size_t imageSize = 0u;
	while (!iStream.eof())
	{
		std::array<PixelComp_t, IJPixel::k_compCount> rawPixel;
		iStream.read((char*)&rawPixel.front(), IJPixel::k_compCount);
		IJPixel* pixel = new IJPixel();
		assert(pixel);
		pixel->deserialize(rawPixel);
		AddPixel(pixel);
		imageSize += IJPixel::k_compCount;
	}

	SetSize(imageSize);
	return IJResult::Success;
}

IJResult IJImage::Load(const std::vector<PixelComp_t>& rawImage)
{
	assert(!rawImage.empty());
	assert(GetPixelData().empty());

	std::strstream stream((char*)&rawImage.front(), rawImage.size(), std::ios::in);
	return Load(stream);
}

IJResult IJImage::Save(std::ostream& oStream) 
{
	if (GetPixelData().empty())
	{
		return IJResult::ImageIsEmpty;
	}

	_SaveHeader(oStream);
	for (size_t i = 0u, size = GetPixelData().size(); i < size; i++)
	{
		std::vector<PixelComp_t> rawPixel = GetPixelData()[i]->serialize();
		oStream.write((char*)&rawPixel.front(), IJPixel::k_compCount);
	}

	return IJResult::Success;
}

IJResult IJImage::Save(std::vector<PixelComp_t>& rawImage)
{
	assert(!GetPixelData().empty());
	if (GetPixelData().empty())
	{
		return IJResult::ImageIsEmpty;
	}

	auto it = GetPixelData().begin();
	while (it != GetPixelData().end())
	{
		std::vector<PixelComp_t> rawPixel = (*it)->serialize();
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
	size_t imageSize = 0u;
	while (!iStream.eof())
	{
		std::array<PixelComp_t, IJPixelAlpha::k_compCount> rawPixel;
		iStream.read((char*)&rawPixel.front(), IJPixel::k_compCount);
		IJPixelAlpha* pixel = new IJPixelAlpha();
		assert(pixel);
		pixel->deserialize(rawPixel);
		AddPixel(pixel);
		imageSize += IJPixel::k_compCount;
	}

	SetSize(imageSize);
	return IJResult::Success;
}

IJResult IJImageAlpha::Load(const std::vector<PixelComp_t>& rawImage)
{
	assert(!rawImage.empty());
	assert(GetPixelData().empty());

	std::strstream stream((char*)&rawImage.front(), rawImage.size(), std::ios::in);
	return Load(stream);
}

IJResult IJImageAlpha::Save(std::ostream& oStream)
{
	if (GetPixelData().empty())
	{
		return IJResult::ImageIsEmpty;
	}

	for (size_t i = 0, size = GetPixelData().size(); i < size; ++i)
	{
		std::vector<PixelComp_t> rawPixel = GetPixelData()[i]->serialize();
		oStream.write((char*)&rawPixel.front(), IJPixel::k_compCount);
	}
	return IJResult::Success;
}

IJResult IJImageAlpha::Save(std::vector<PixelComp_t>& rawImage)
{
	assert(!GetPixelData().empty());
	if (GetPixelData().empty())
	{
		return IJResult::ImageIsEmpty;
	}

	auto it = GetPixelData().begin();
	while (it != GetPixelData().end())
	{
		std::vector<PixelComp_t> rawPixel = (*it)->serialize();
		rawImage.insert(rawImage.end(), rawPixel.begin(), rawPixel.end());
		++it;
	}

	return IJResult::Success;
}
