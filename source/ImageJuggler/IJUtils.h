#pragma once

#include "IJTypes.h"

namespace cusmath
{
	template <typename T>
	T clamp(const T& val, const T& lower, const T& upper)
	{
		if (val < lower)
		{
			return lower;
		}
		else if (val > upper) 
		{
			return upper;
		}
		else
			return val;
	}
}
