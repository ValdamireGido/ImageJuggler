#include "IJImage.h"
#include <assert.h>
#include <strstream>


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
		rawImage.push_back(it->c1);
		rawImage.push_back(it->c2);
		rawImage.push_back(it->c3);
		++it;
	}

	return IJResult::Success;
}


/*
		IJImageAlpha realization
*/

IJResult IJImageAlpha::Load(std::istream& iStream)
{
	iStream.seekg(0, iStream.end);
	int streamSize = iStream.tellg();
	iStream.seekg(18, iStream.beg);

	size_t size = streamSize / IJPixelAlpha::k_compCount;
	Resize(size);
	iStream.read((char*)&GetData().front(), streamSize);
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

	oStream.write((const char*)&GetData().front(), GetPixelCount() * IJPixelAlpha::k_compCount);
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
		rawImage.push_back(it->c1);
		rawImage.push_back(it->c2);
		rawImage.push_back(it->c3);
		rawImage.push_back(it->a);
		++it;
	}

	return IJResult::Success;
}
