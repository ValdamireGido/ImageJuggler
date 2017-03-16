#pragma once 

#include "IJImage.h"
#include <array>

/*
		YCbCr default image 
		Has by 8 bits per every chanel
*/

class IJYCbCrPixel888 : public IJPixel {};
class IJYCbCrImage888
	: public IJImage
{
public:
	IJYCbCrImage888();
	IJYCbCrImage888(const std::string& fileName);
	~IJYCbCrImage888();
};

