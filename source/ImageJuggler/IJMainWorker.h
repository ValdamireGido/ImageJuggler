#pragma once 

#include "IJTypes.h"
#include <string>

class IJMainWorker
{
public:
	// C'tors
	IJMainWorker() = default;
	IJMainWorker(const IJMainWorker&&) = delete;
	IJMainWorker(IJMainWorker&) = delete;
	IJMainWorker& operator=(const IJMainWorker&&) = delete;
	IJMainWorker& operator=(IJMainWorker&) = delete;

	// D'tor
	~IJMainWorker() = default;

	// Class Interface
	IJResult		Load(const std::string& fileName);
	//IJResult		LoadRGB(const std::string& fileName);
	//IJResult		LoadYCbCr(const std::string& fileName);
	//IJResult		LoadYCoCg(const std::string& fileName);
};