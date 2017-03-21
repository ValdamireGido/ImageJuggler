#pragma once

#include "IJTypes.h"

#include <functional>
#include <thread>
#include <future>

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

namespace parallel
{
	inline void asyncForeach(int _start,
							 int _end,
							 std::function<void(int _i)> _func, 
							 const size_t _nThreads = 4)
	{
		if (!_func)
		{
			return;
		}

		if (_start == _end)
		{
			return;
		}

		std::vector<std::future<void> > handles;
		handles.resize(_nThreads);
		int size = _end - _start;
		int sizePerThread = size / _nThreads;
		for (size_t handleIdx = 0; handleIdx < _nThreads; handleIdx++)
		{
			size_t offset = sizePerThread * handleIdx;
			size_t start = _start + offset;
			size_t end = start + sizePerThread;
			handles[handleIdx] = std::async(std::launch::async, 
											[start, end, &_func] () 
			{
				for (size_t i = start; i < end; i++)
				{
					_func(i);
				}
			});
		}

		for (auto& handle : handles)
		{
			handle.wait();
		}
	}
}