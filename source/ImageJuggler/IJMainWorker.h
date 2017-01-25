#pragma once 

#include "IJTypes.h"
#include <string>

class LCMainWorker
{
public:
	// C'tors
	LCMainWorker() = default;
	LCMainWorker(const LCMainWorker&&) = delete;
	LCMainWorker(LCMainWorker&) = delete;
	LCMainWorker& operator=(const LCMainWorker&&) = delete;
	LCMainWorker& operator=(LCMainWorker&) = delete;

	// D'tor
	~LCMainWorker() {}

	// Class Interface
	IJResult		Load(const std::string& fileName);
	//IJResult		LoadRGB(const std::string& fileName);
	//IJResult		LoadYCbCr(const std::string& fileName);
	//IJResult		LoadYCoCg(const std::string& fileName);
};