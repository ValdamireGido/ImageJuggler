#include "IJPackedColorImage.h"
#include "IJYCbCrImage888.h"
#include "IJUtils.h"

#include <assert.h>

IJPackedColorImage::IJPackedColorImage() 
	: m_yImage(nullptr)
	, m_uImage(nullptr)
	, m_vImage(nullptr)
	, m_packRate(2.f)
	, m_fileName()
{}

IJPackedColorImage::IJPackedColorImage(const std::string& fileName)
	: m_yImage(nullptr)
	, m_uImage(nullptr)
	, m_vImage(nullptr)
	, m_packRate(2.f)
	, m_fileName(fileName)
{}

IJPackedColorImage::IJPackedColorImage(const std::string& fileName, float packRate)
	: m_yImage(nullptr)
	, m_uImage(nullptr)
	, m_vImage(nullptr)
	, m_packRate(packRate)
	, m_fileName(fileName)
{}

IJPackedColorImage::~IJPackedColorImage() 
{}

IJResult IJPackedColorImage::Load(std::istream& istream)
{
	if (!m_yImage)
	{
		CreateCompImages();
	}

	IJResult result = IJResult::UnknownResult;
	result = m_yImage->Load(istream);
	assert(result == IJResult::Success);

	result = m_uImage->Load(istream);
	assert(result == IJResult::Success);

	result = m_vImage->Load(istream);
	assert(result == IJResult::Success);

	return result;
}

IJResult IJPackedColorImage::Save(std::ostream& ostream)
{
	ASSERT_PTR(m_yImage);
	ASSERT_PTR(m_uImage);
	ASSERT_PTR(m_vImage);

	IJResult result = IJResult::UnknownResult;
	result = m_yImage->Save(ostream);
	assert(result == IJResult::Success);

	result = m_uImage->Save(ostream);
	assert(result == IJResult::Success);

	result = m_vImage->Save(ostream);
	assert(result == IJResult::Success);

	return result;
}

IJResult IJPackedColorImage::PackImage(IJYCbCrImage888* image, float rate)
{
	ASSERT_PTR(image);
	
	if (!m_yImage)
	{
		CreateCompImages();
	}

	if (rate != GetPackRate())
	{
		SetPackRate(rate);
	}

	size_t  ySize = image->GetSize(), 
			uvSize = ySize / m_packRate;


}

/*
		Private methods realization
*/

void IJPackedColorImage::CreateCompImages()
{
	ASSERT_PTR_VOID(!m_yImage);
	ASSERT_PTR_VOID(!m_uImage);
	ASSERT_PTR_VOID(!m_vImage);

	m_yImage = new IJSingleCompImage();
	assert(m_yImage);

	m_uImage = new IJSingleCompImage();
	assert(m_uImage);

	m_vImage = new IJSingleCompImage();
	assert(m_vImage);
}
