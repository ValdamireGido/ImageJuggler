#include "IJImage.h"

std::string GetFileExtension(const std::string& fileName) 
{
	std::string ext;
	size_t pos = fileName.find('.');
	if (pos != std::string::npos)
	{
		ext = fileName.substr(pos);
	}

	return ext;
}

