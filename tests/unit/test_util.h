/*
 * test_util.h:
 * Utility functions and wrappers shared amonst testing
 * translation units
 *
 * See LICENCE.md for Copyright information
 */

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
