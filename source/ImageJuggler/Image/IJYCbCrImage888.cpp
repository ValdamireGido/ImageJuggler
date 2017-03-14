#include "IJYCbCrImage888.h"

IJYCbCrImage888::IJYCbCrImage888()
	: IJImage(IJImageType::YCbCr888)
{
}

IJYCbCrImage888::IJYCbCrImage888(const std::string& fileName)
	: IJImage(fileName, IJImageType::YCbCr888)
{
}

IJYCbCrImage888::~IJYCbCrImage888()
{
}
