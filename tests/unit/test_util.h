/*
 * test_util.h:
 * Utility functions and wrappers shared amonst testing
 * translation units
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_TEST_UTIL_H
#define YIQI_TEST_UTIL_H

#include <vector>

namespace yiqi
{
    namespace testing
    {
	/**
	 * @brief Copy array into vector so we
	 * can use it with Google Test matchers
	 */
	template <typename T>
	std::vector <T> ToVector (T const *array, size_t size)
	{
	    if (size)
		return std::vector <T> (array, array + size);
	    else
		throw std::out_of_range ("ToVector: size is < 1");
	}
    }
}

#endif // YIQI_TEST_UTIL_H
