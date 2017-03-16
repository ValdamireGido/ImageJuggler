#pragma once

#include "IJTypes.h"

#define ASSERT_PTR(ptr)\
do {\
	assert(ptr);\
	if (!ptr) {\
		return IJResult::BadMemoryPointer;\
	}\
}while(false)

#define ASSERT_PTR_VOID(ptr)\
do {\
	assert(ptr);\
	if (!ptr) {\
		return;\
	}\
} while (false)

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
