#include "IJSingleCompImage.h"
#include <assert.h>


/* 
		YUV Single component image 
*/

IJSingleCompImage::IJSingleCompImage()
	: IJImageInterface<uint8_t, 1u>(IJImageType::SinglComp)
{}

IJResult IJSingleCompImage::Load(std::istream& iStream)
{
	iStream.seekg(std::ios::end);
	size_t size = (size_t)iStream.tellg();
	iStream.seekg(std::ios::beg);

	GetData().resize(size);
	iStream.read((char*)&GetData().front(), size);
	
	return IJResult::Success;
}

IJResult IJSingleCompImage::Load(std::istream& istream, size_t size)
{
	GetData().resize(size);
	istream.read((char*)&GetData().front(), size);
	
	return IJResult::Success;
}

IJResult IJSingleCompImage::Save(std::ostream& oStream)
{
	assert(!GetData().empty());
	if (GetData().empty())
	{
		return IJResult::ImageIsEmpty;
	}

	oStream.write((const char*)&GetData().front(), GetData().size());
	return IJResult::Success;
}

IJResult IJSingleCompImage::DebugSave(const std::string& fileName, uint8_t compsToDump)
{
	std::ofstream ofile(fileName, std::ios::out | std::ios::binary);
	SaveHeader(ofile);
	for (size_t i = 0, size = GetSize(); i < size; i++)
	{
		static char zeroStr = 0;
		if ((compsToDump & B) == B)
			ofile.write((char*)&GetData()[i], 1);
		else 
			ofile.write(&zeroStr, 1);

		if ((compsToDump & G) == G)
			ofile.write((char*)&GetData()[i], 1);
		else 
			ofile.write(&zeroStr, 1);

		if ((compsToDump& R) == R)
			ofile.write((char*)&GetData()[i], 1);
		else 
			ofile.write(&zeroStr, 1);
	}

	ofile.close();
	return IJResult::Success;
}
