#pragma once 

#include "Types.h"
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
	Result		Load(const std::string& fileName);
	//Result		LoadRGB(const std::string& fileName);
	//Result		LoadYCbCr(const std::string& fileName);
	//Result		LoadYCoCg(const std::string& fileName);
};